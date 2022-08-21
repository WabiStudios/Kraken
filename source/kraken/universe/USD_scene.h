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
 * @file USD_scene.h
 * @ingroup UNI
 */

#pragma once

#include "USD_api.h"
#include "USD_object.h"

#include "KKE_version.h"

#include <wabi/base/gf/vec2f.h>
#include <wabi/base/gf/vec4d.h>
#include <wabi/base/gf/vec4f.h>

#include <wabi/base/tf/token.h>

#include <wabi/usd/usd/stage.h>
#include <wabi/usd/usdGeom/metrics.h>
#include <wabi/usd/usdLux/domeLight.h>
#include <wabi/imaging/hdx/tokens.h>

WABI_NAMESPACE_BEGIN

/**
 * ---------------------------------------------------------------------
 *  ::::::::        :       :      :     :    :  :::::: UNI::SCENE
 * ---------------------------------------------------------------------
 */

enum eSceneDrawMode
{
  DRAW_POINTS,
  DRAW_WIREFRAME,
  DRAW_WIREFRAME_ON_SURFACE,
  DRAW_SHADED_FLAT,
  DRAW_SHADED_SMOOTH,
  DRAW_GEOM_ONLY,
  DRAW_GEOM_FLAT,
  DRAW_GEOM_SMOOTH
};

enum eSceneCullStyle
{
  CULL_STYLE_NO_OPINION,
  CULL_STYLE_NOTHING,
  CULL_STYLE_BACK,
  CULL_STYLE_FRONT,
  CULL_STYLE_BACK_UNLESS_DOUBLE_SIDED,

  CULL_STYLE_COUNT
};

/**
 * GEOMETRY: PRIMITIVES
 * Geometric primitives,
 * which a user can provide
 * to the active stage via
 * shift+a.
 */

enum eScenePrimitives
{
  MESH_CUBE = 0,
  MESH_SPHERE,
  MESH_GRID,
  MESH_CONE,
  MESH_CYLINDER,
  MESH_CAPSULE,
  MESH_SUZANNE
};

/**
 * ENGINES: SELECTION
 * Compatible Render Engines
 * enumerated in the same
 * order as they are provided
 * from the plugins directory.
 */

enum eSceneRenderEngine
{
  ENGINE_PHOENIX = 0,
  ENGINE_ARNOLD,
  ENGINE_EMBREE,
  ENGINE_RENDERMAN,

  ENGINE_MAX,
};

enum eSceneLoadSet
{
  SCENE_LOAD_ALL,
  SCENE_LOAD_NONE
};

struct Scene
{
  explicit Scene(const UsdStageRefPtr &stage);
  explicit Scene(const std::string &identifier, const UsdPrim &prim = UsdPrim());
  explicit Scene(const std::string &identifier, const UsdSchemaBase &schemaObj);
  virtual ~Scene();

  /** This scenes active stage. */
  UsdStageRefPtr stage;
};

WABI_NAMESPACE_END