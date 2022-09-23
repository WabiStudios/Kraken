/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * Copyright 2022, Wabi Animation Studios, Ltd. Co.
 */

/**
 * @file
 * KRAKEN Library.
 * Gadget Vault.
 */

#  include <stdlib.h> /* malloc */
#  include <string.h>

#  include <fcntl.h>
#  include <sys/stat.h>
#  include <sys/types.h>

#  include <errno.h>

#include <zlib.h>
#include <zstd.h>

#ifdef WIN32
#  include "KLI_fileops_types.h"
#  include "KLI_winstuff.h"
#  include "utf_winfunc.h"
#  include "utfconv.h"
#  include <io.h>
#  include <shellapi.h>
#  include <shobjidl.h>
#  include <windows.h>
#else
#  if defined(__APPLE__)
#    include <CoreFoundation/CoreFoundation.h>
#    include <objc/message.h>
#    include <objc/runtime.h>
#  endif
#  include <dirent.h>
#  include <sys/param.h>
#  include <sys/wait.h>
#  include <unistd.h>
#endif

#include "KLI_fileops.h"
#include "KLI_path_utils.h"
#include "KLI_string.h"
#include "KLI_sys_types.h" /* for intptr_t support */
#include "KLI_utildefines.h"


KRAKEN_NAMESPACE_BEGIN

/* results from recursive_operation and its callbacks */
enum
{
  /* operation succeeded */
  RecursiveOp_Callback_OK = 0,

  /* operation requested not to perform recursive digging for current path */
  RecursiveOp_Callback_StopRecurs = 1,

  /* error occurred in callback and recursive walking should stop immediately */
  RecursiveOp_Callback_Error = 2,
};

static int delete_callback_post(const char *from, const char *UNUSED(to))
{
  if (rmdir(from)) {
    perror("rmdir");

    return RecursiveOp_Callback_Error;
  }

  return RecursiveOp_Callback_OK;
}

static int delete_single_file(const char *from, const char *UNUSED(to))
{
  if (unlink(from)) {
    perror("unlink");

    return RecursiveOp_Callback_Error;
  }

  return RecursiveOp_Callback_OK;
}


#  ifdef WIN32
void KLI_get_short_name(char short_name[256], const char *filepath)
{
  wchar_t short_name_16[256];
  int i = 0;

  UTF16_ENCODE(filepath);

  GetShortPathNameW(filepath_16, short_name_16, 256);

  for (i = 0; i < 256; i++) {
    short_name[i] = (char)short_name_16[i];
  }

  UTF16_UN_ENCODE(filepath);
}

int uaccess(const char *filename, int mode)
{
  int r = -1;
  UTF16_ENCODE(filename);

  if (filename_16) {
    r = _waccess(filename_16, mode);
  }

  UTF16_UN_ENCODE(filename);

  return r;
}

int KLI_access(const char *filename, int mode)
{
  KLI_assert(!KLI_path_is_rel(filename));

  return uaccess(filename, mode);
}

/** @return true on success (i.e. given path now exists on FS), false otherwise. */
bool KLI_dir_create_recursive(const char *dirname)
{
  char *lslash;
  char tmp[MAXPATHLEN];
  bool ret = true;

  /* First remove possible slash at the end of the dirname.
   * This routine otherwise tries to create
   * blah1/blah2/ (with slash) after creating
   * blah1/blah2 (without slash) */

  KLI_strncpy(tmp, dirname, sizeof(tmp));
  KLI_path_slash_rstrip(tmp);

  /* check special case "c:\foo", don't try create "c:", harmless but prints an error below */
  if (isalpha(tmp[0]) && (tmp[1] == ':') && tmp[2] == '\0') {
    return true;
  }

  if (KLI_is_dir(tmp)) {
    return true;
  } else if (KLI_exists(tmp)) {
    return false;
  }

  lslash = (char *)KLI_path_slash_rfind(tmp);

  if (lslash) {
    /* Split about the last slash and recurse */
    *lslash = 0;
    if (!KLI_dir_create_recursive(tmp)) {
      ret = false;
    }
  }

  if (ret && dirname[0]) { /* patch, this recursive loop tries to create a nameless directory */
    if (umkdir(dirname) == -1) {
      printf("Unable to create directory %s\n", dirname);
      ret = false;
    }
  }
  return ret;
}

static void callLocalErrorCallBack(const char *err)
{
  TF_WARN("%s\n", err);
}

static bool delete_unique(const char *path, const bool dir)
{
  bool err;

  UTF16_ENCODE(path);

  if (dir) {
    err = !RemoveDirectoryW(path_16);
    if (err) {
      printf("Unable to remove directory\n");
    }
  } else {
    err = !DeleteFileW(path_16);
    if (err) {
      callLocalErrorCallBack("Unable to delete file");
    }
  }

  UTF16_UN_ENCODE(path);

  return err;
}

static bool delete_recursive(const char *dir)
{
  struct direntry *filelist, *fl;
  bool err = false;
  uint nbr, i;

  i = nbr = KLI_filelist_dir_contents(dir, &filelist);
  fl = filelist;
  while (i--) {
    const char *file = KLI_path_basename(fl->path);

    if (FILENAME_IS_CURRPAR(file)) {
      /* Skip! */
    } else if (S_ISDIR(fl->type)) {
      char path[FILE_MAXDIR];

      /* dir listing produces dir path without trailing slash... */
      KLI_strncpy(path, fl->path, sizeof(path));
      KLI_path_slash_ensure(path);

      if (delete_recursive(path)) {
        err = true;
      }
    } else {
      if (delete_unique(fl->path, false)) {
        err = true;
      }
    }
    fl++;
  }

  if (!err && delete_unique(dir, true)) {
    err = true;
  }

  KLI_filelist_free(filelist, nbr);

  return err;
}

int KLI_delete(const char *file, bool dir, bool recursive)
{
  int err;

  KLI_assert(!KLI_path_is_rel(file));

  if (recursive) {
    err = delete_recursive(file);
  } else {
    err = delete_unique(file, dir);
  }

  return err;
}
#  endif /* WIN32 */
static int delete_soft(const char *file, const char **error_message)
{
  int ret = -1;

  Class NSAutoreleasePoolClass = objc_getClass("NSAutoreleasePool");
  SEL allocSel = sel_registerName("alloc");
  SEL initSel = sel_registerName("init");
  id poolAlloc = ((id(*)(Class, SEL))objc_msgSend)(NSAutoreleasePoolClass, allocSel);
  id pool = ((id(*)(id, SEL))objc_msgSend)(poolAlloc, initSel);

  Class NSStringClass = objc_getClass("NSString");
  SEL stringWithUTF8StringSel = sel_registerName("stringWithUTF8String:");
  id pathString = ((
    id(*)(Class, SEL, const char *))objc_msgSend)(NSStringClass, stringWithUTF8StringSel, file);

  Class NSFileManagerClass = objc_getClass("NSFileManager");
  SEL defaultManagerSel = sel_registerName("defaultManager");
  id fileManager = ((id(*)(Class, SEL))objc_msgSend)(NSFileManagerClass, defaultManagerSel);

  Class NSURLClass = objc_getClass("NSURL");
  SEL fileURLWithPathSel = sel_registerName("fileURLWithPath:");
  id nsurl = ((id(*)(Class, SEL, id))objc_msgSend)(NSURLClass, fileURLWithPathSel, pathString);

  SEL trashItemAtURLSel = sel_registerName("trashItemAtURL:resultingItemURL:error:");
  BOOL deleteSuccessful = ((
    BOOL(*)(id, SEL, id, id, id))objc_msgSend)(fileManager, trashItemAtURLSel, nsurl, nil, nil);

  if (deleteSuccessful) {
    ret = 0;
  } else {
    *error_message = "The Cocoa API call to delete file or directory failed";
  }

  SEL drainSel = sel_registerName("drain");
  ((void (*)(id, SEL))objc_msgSend)(pool, drainSel);

  return ret;
}

/** \return true on success (i.e. given path now exists on FS), false otherwise. */
bool KLI_dir_create_recursive(const char *dirname)
{
  char *lslash;
  size_t size;
#  ifdef MAXPATHLEN
  char static_buf[MAXPATHLEN];
#  endif
  char *tmp;
  bool ret = true;

  if (KLI_is_dir(dirname)) {
    return true;
  }
  if (KLI_exists(dirname)) {
    return false;
  }

#  ifdef MAXPATHLEN
  size = MAXPATHLEN;
  tmp = static_buf;
#  else
  size = strlen(dirname) + 1;
  tmp = calloc(size, __func__);
#  endif

  KLI_strncpy(tmp, dirname, size);

  /* Avoids one useless recursion in case of '/foo/bar/' path... */
  KLI_path_slash_rstrip(tmp);

  lslash = (char *)KLI_path_slash_rfind(tmp);
  if (lslash) {
    /* Split about the last slash and recurse */
    *lslash = 0;
    if (!KLI_dir_create_recursive(tmp)) {
      ret = false;
    }
  }

#  ifndef MAXPATHLEN
  free(tmp);
#  endif

  if (ret) {
    ret = (mkdir(dirname, 0777) == 0);
  }
  return ret;
}

typedef int (*RecursiveOp_Callback)(const char *from, const char *to);

/* appending of filename to dir (ensures for buffer size before appending) */
static void join_dirfile_alloc(char **dst, size_t *alloc_len, const char *dir, const char *file)
{
  size_t len = strlen(dir) + strlen(file) + 1;

  if (*dst == NULL) {
    *dst = (char *)malloc(len + 1);
  } else if (*alloc_len < len) {
    *dst = (char *)realloc(*dst, len + 1);
  }

  *alloc_len = len;

  KLI_join_dirfile(*dst, len + 1, dir, file);
}

static char *strip_last_slash(const char *dir)
{
  char *result = KLI_strdup(dir);
  KLI_path_slash_rstrip(result);

  return result;
}


/**
 * Scans @a startfrom, generating a corresponding destination name for each item found by
 * prefixing it with startto, recursively scanning subdirectories, and invoking the specified
 * callbacks for files and subdirectories found as appropriate.
 *
 * @param startfrom: Top-level source path.
 * @param startto: Top-level destination path.
 * @param callback_dir_pre: Optional, to be invoked before entering a subdirectory, can return
 *                          RecursiveOp_Callback_StopRecurs to skip the subdirectory.
 * @param callback_file: Optional, to be invoked on each file found.
 * @param callback_dir_post: optional, to be invoked after leaving a subdirectory.
 * @return */
static int recursive_operation(const char *startfrom,
                               const char *startto,
                               RecursiveOp_Callback callback_dir_pre,
                               RecursiveOp_Callback callback_file,
                               RecursiveOp_Callback callback_dir_post)
{
  struct stat st;
  char *from = NULL, *to = NULL;
  char *from_path = NULL, *to_path = NULL;
  struct dirent **dirlist = NULL;
  size_t from_alloc_len = -1, to_alloc_len = -1;
  int i, n = 0, ret = 0;

  do { /* once */
    /* ensure there's no trailing slash in file path */
    from = strip_last_slash(startfrom);
    if (startto) {
      to = strip_last_slash(startto);
    }

    ret = lstat(from, &st);
    if (ret < 0) {
      /* source wasn't found, nothing to operate with */
      break;
    }

    if (!S_ISDIR(st.st_mode)) {
      /* source isn't a directory, can't do recursive walking for it,
       * so just call file callback and leave */
      if (callback_file != NULL) {
        ret = callback_file(from, to);
        if (ret != RecursiveOp_Callback_OK) {
          ret = -1;
        }
      }
      break;
    }

    n = scandir(startfrom, &dirlist, NULL, alphasort);
    if (n < 0) {
      /* error opening directory for listing */
      perror("scandir");
      ret = -1;
      break;
    }

    if (callback_dir_pre != NULL) {
      ret = callback_dir_pre(from, to);
      if (ret != RecursiveOp_Callback_OK) {
        if (ret == RecursiveOp_Callback_StopRecurs) {
          /* callback requested not to perform recursive walking, not an error */
          ret = 0;
        } else {
          ret = -1;
        }
        break;
      }
    }

    for (i = 0; i < n; i++) {
      const struct dirent *const dirent = dirlist[i];

      if (FILENAME_IS_CURRPAR(dirent->d_name)) {
        continue;
      }

      join_dirfile_alloc(&from_path, &from_alloc_len, from, dirent->d_name);
      if (to) {
        join_dirfile_alloc(&to_path, &to_alloc_len, to, dirent->d_name);
      }

      bool is_dir;

#  ifdef __HAIKU__
      {
        struct stat st_dir;
        char filename[FILE_MAX];
        KLI_path_join(filename, sizeof(filename), startfrom, dirent->d_name, NULL);
        lstat(filename, &st_dir);
        is_dir = S_ISDIR(st_dir.st_mode);
      }
#  else
      is_dir = (dirent->d_type == DT_DIR);
#  endif

      if (is_dir) {
        /* recursively dig into a subfolder */
        ret = recursive_operation(from_path,
                                  to_path,
                                  callback_dir_pre,
                                  callback_file,
                                  callback_dir_post);
      } else if (callback_file != NULL) {
        ret = callback_file(from_path, to_path);
        if (ret != RecursiveOp_Callback_OK) {
          ret = -1;
        }
      }

      if (ret != 0) {
        break;
      }
    }
    if (ret != 0) {
      break;
    }

    if (callback_dir_post != NULL) {
      ret = callback_dir_post(from, to);
      if (ret != RecursiveOp_Callback_OK) {
        ret = -1;
      }
    }
  } while (false);

  if (dirlist != NULL) {
    for (i = 0; i < n; i++) {
      free(dirlist[i]);
    }
    free(dirlist);
  }
  if (from_path != NULL) {
    free(from_path);
  }
  if (to_path != NULL) {
    free(to_path);
  }
  if (from != NULL) {
    free(from);
  }
  if (to != NULL) {
    free(to);
  }

  return ret;
}


FILE *KLI_fopen(const char *filepath, const char *mode)
{
  KLI_assert(!KLI_path_is_rel(filepath));

  return fopen(filepath, mode);
}

void *KLI_gzopen(const char *filepath, const char *mode)
{
  KLI_assert(!KLI_path_is_rel(filepath));

  return gzopen(filepath, mode);
}

int KLI_open(const char *filepath, int oflag, int pmode)
{
  KLI_assert(!KLI_path_is_rel(filepath));

  return open(filepath, oflag, pmode);
}

int KLI_access(const char *filepath, int mode)
{
  KLI_assert(!KLI_path_is_rel(filepath));

  return access(filepath, mode);
}

int KLI_delete(const char *file, bool dir, bool recursive)
{
  KLI_assert(!KLI_path_is_rel(file));

  if (recursive) {
    return recursive_operation(file, NULL, NULL, delete_single_file, delete_callback_post);
  }
  if (dir) {
    return rmdir(file);
  }
  return remove(file);
}


KRAKEN_NAMESPACE_END