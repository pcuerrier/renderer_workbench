#pragma once

#include "core.h"
#include "core/memory.h"
#include "renderer/renderer.h"

RenderQueue AppUpdate(Memory& app_memory, RenderQueue& render_queue, const Input& curr_input, const Input& old_input, float width, float height, float delta_time);
