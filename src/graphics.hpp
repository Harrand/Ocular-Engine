#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "utility.hpp"

namespace tz
{
	namespace graphics
	{
		extern bool has_context;
		extern bool initialised;
		inline void initialise()
		{
			if(!tz::graphics::has_context)
			{
				tz::util::log::warning("Initialisation of tz::graphics aborted: No OpenGL context has been setup yet.");
				return;
			}
			tz::util::log::message("OpenGL context detected, initialising tz::graphics...");
			GLenum status = glewInit();
			if(status != GLEW_OK)
				tz::util::log::error("Initialisation of GLEW failed.\n\t_initialisation of tz::graphics unsuccessful!");
			else
				tz::util::log::message("Initialisation of GLEW successful.\n\t_initialised tz::graphics via GLEW (OpenGL).");
			if(TTF_Init() == 0)
				tz::util::log::message("Successfully initialised SDL2_ttf.");
			else
				tz::util::log::error("Initialisation of SDL2_ttf failed.");
			tz::graphics::initialised = true;
			glEnable(GL_FRAMEBUFFER_SRGB);
			glEnable(GL_MULTISAMPLE);
		}
		inline void terminate()
		{
			TTF_Quit();
			tz::util::log::message("Terminated SDL2_ttf");
			tz::util::log::message("Terminated tz::graphics via GLEW (OpenGL).");
		}
		
		namespace model
		{
			class OBJIndex
			{
			public:
				unsigned int vertex_index;
				unsigned int uv_index;
				unsigned int normal_index;
				bool operator<(const OBJIndex& rhs) const { return vertex_index < rhs.vertex_index; }
			};
			
			class IndexedModel
			{
			public:
				std::vector<Vector3F> positions;
				std::vector<Vector2F> texcoords;
				std::vector<Vector3F> normals;
				std::vector<Vector3F> tangents;
				std::vector<unsigned int> indices;
				void calculate_normals();
				void calculate_tangents();
			};
			
			class OBJModel
			{
			public:
				std::vector<OBJIndex> obj_indices;
				std::vector<Vector3F> vertices;
				std::vector<Vector2F> uvs;
				std::vector<Vector3F> normals;
				bool has_uvs;
				bool has_normals;
				
				OBJModel(const std::string& file_name);
				
				IndexedModel to_indexed_model();
			private:
				unsigned int find_last_vertex_index(const std::vector<OBJIndex*>& index_lookup, const OBJIndex* current_index, const IndexedModel& result);
				void create_o_b_j_face(const std::string& line);
				
				Vector2F parse_o_b_j_vector_2_f(const std::string& line);
				Vector3F parse_o_b_j_vector_3_f(const std::string& line);
				OBJIndex parse_o_b_j_index(const std::string& token, bool* has_uvs, bool* has_normals);
			};
		}
	}
}

class Vertex
{
public:
	Vertex(Vector3F position, Vector2F texcoord, Vector3F normal);
	Vertex(const Vertex& copy) = default;
	Vertex(Vertex&& move) = default;
	~Vertex() = default;
	Vertex& operator=(const Vertex& rhs) = default;
	
	const Vector3F& get_position() const;
	const Vector2F& get_texture_coordinate() const;
	const Vector3F& get_normal() const;
private:
	const Vector3F position;
	const Vector2F texcoord;
	const Vector3F normal;
};

class Font
{
public:
	Font(const std::string& font_path, int pixel_height);
	Font(const Font& copy);
	Font(Font&& move);
	~Font();
	Font& operator=(Font&& rhs); // move assignment operator
	
	TTF_Font* get_font_handle() const;
private:
	std::string font_path;
	int pixel_height;
	TTF_Font* font_handle;
};

#endif