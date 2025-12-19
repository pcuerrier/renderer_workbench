#pragma once

#include "core.h"
#include "renderer/renderer.h"
#include "app.h"

struct Entity {
    //vec3 position;
    //vec3 rotation;
    MeshHandle mesh_id;
    //TextureHandle texture_id;
    bool is_visible;
};
