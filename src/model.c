#include "model.h"
#include "camera.h"

#define CGLTF_IMPLEMENTATION
#include "cgltf/cgltf.h"

void create_model(model_t* model) {
	glGenVertexArrays(1, &model->vao);
	glGenBuffers(1, &model->vbo);
	glGenBuffers(1, &model->ebo);

	glBindVertexArray(model->vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, model->vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_t)*model->vertex_count, model->vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*model->indice_count, model->indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, uv));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex_t), (void*)offsetof(vertex_t, normal));
	glEnableVertexAttribArray(2);
}

uint8_t load_model(const char* path, model_t* model) {
	cgltf_options options = {0};
	cgltf_data* data = NULL;
	cgltf_result result = cgltf_parse_file(&options, path, &data);
	
	if (result != cgltf_result_success) {
		return 0;
	} else if (cgltf_load_buffers(&options, data, path) != cgltf_result_success) {
		return 0;
	} else if (cgltf_validate(data) != cgltf_result_success) {
		return 0;
	}

	const cgltf_mesh* mesh = &data->meshes[0];
	const cgltf_primitive* prim = &mesh->primitives[0];

	cgltf_accessor* pos_acc = NULL;
	cgltf_accessor* norm_acc = NULL;
	cgltf_accessor* texcoord_acc = NULL;

	for (size_t i = 0; i < prim->attributes_count; ++i) {
		cgltf_attribute attr = prim->attributes[i];
		if (attr.type == cgltf_attribute_type_position) {
			pos_acc = attr.data;
		} else if (attr.type == cgltf_attribute_type_normal) {
			norm_acc = attr.data;
		} else if (attr.type == cgltf_attribute_type_texcoord) {
			texcoord_acc = attr.data;
		}
	}

	if (!pos_acc || pos_acc->count == 0) {
		return 0;
	}

	model->vertex_count = (unsigned int)pos_acc->count;
	model->vertices = malloc(sizeof(vertex_t)*model->vertex_count);

	for (size_t i = 0; i < model->vertex_count; ++i) {
		vertex_t* v = &model->vertices[i];

		cgltf_accessor_read_float(pos_acc, i, v->position, 3);
		if (norm_acc) {
			cgltf_accessor_read_float(norm_acc, i, v->normal, 3);
		} if (texcoord_acc) {
			cgltf_accessor_read_float(texcoord_acc, i, v->uv, 2);
		}
	}

	if (prim->indices) {
		model->indice_count = (unsigned int)prim->indices->count;
		model->indices = malloc(sizeof(unsigned int) * model->indice_count);

		for (size_t i = 0; i < model->indice_count; ++i) {
			model->indices[i] = (unsigned int)cgltf_accessor_read_index(prim->indices, i);
		}
	}

	create_model(model);
	cgltf_free(data);
	return 1;
}
