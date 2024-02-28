#pragma once

#include "Source/VulkanMonotony.h"
#define CGLTF_IMPLEMENTATION
#include <CGLTF/cgltf.h>
#include <vector>

class modelReader
{
private:
	cgltf_data* data = 0;
	uint32_t indicesCount;
	uint64_t indexDataSize;
	void* indexData;
	uint32_t vertexDataSize;
	void* vertexData;
public:
	void load(const char* filename)
	{
		cgltf_options options = {};
		cgltf_result error = cgltf_parse_file(&options, filename, &data);
		if (error == cgltf_result_success)
		{
			error = cgltf_load_buffers(&options, data, "Models");
			if (error == cgltf_result_success)
			{
				//assert(data->meshes_count >= 1);
				//assert(data->meshes[0].primitives_count == 1);
				//assert(data->meshes[0].primitives[0].attributes_count > 0);
				//assert(data->meshes[0].primitives[0].indices->component_type == cgltf_component_type_r_32u);
				//assert(data->meshes[0].primitives[0].indices->stride == sizeof(uint32_t));

				uint8_t* bufferBase = (uint8_t*)data->meshes[0].primitives[0].indices->buffer_view->buffer->data;
				indexDataSize = data->meshes[0].primitives[0].indices->buffer_view->size;
				indexData = bufferBase + data->meshes[0].primitives[0].indices->buffer_view->offset;
				indicesCount = data->meshes[0].primitives[0].indices->count;

				bufferBase = (uint8_t*)data->meshes[0].primitives[0].attributes[0].data->buffer_view->buffer->data;
				vertexDataSize = data->meshes[0].primitives[0].attributes[0].data->buffer_view->size;
				vertexData = bufferBase + data->meshes[0].primitives[0].attributes[0].data->buffer_view->offset;
			}
		}
	}
	uint32_t getIndicesCount()
	{
		return indicesCount;
	}

	uint64_t getIndexSize()
	{
		return indexDataSize;
	}

	void* getIndexData()
	{
		return indexData;
	}

	uint32_t getVertexSize()
	{
		return vertexDataSize;
	}

	void* getVertexData()
	{
		return vertexData;
	}

	void destroyModel()
	{
		cgltf_free(data);
	}
};