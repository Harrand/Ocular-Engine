#include "graphics/texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "graphics/stb_image.h"

Texture::Texture():Texture(0, 0, false){}

// This is private, to use this constructor, call it via Texture(int width, int height)
Texture::Texture(int width, int height, bool initialise_handle, tz::graphics::TextureComponent texture_component): texture_handle(0), width(width), height(height), components(0), texture_component(texture_component), gamma_corrected(false), bitmap({})
{
	if(initialise_handle)
	{
		// Generates a new texture, and just fills it with zeroes if specified.
		glGenTextures(1, &(this->texture_handle));
		glBindTexture(GL_TEXTURE_2D, this->texture_handle);
		switch(this->texture_component)
		{
			case tz::graphics::TextureComponent::COLOUR_TEXTURE:
			    this->components = 4;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
				break;
			case tz::graphics::TextureComponent::HDR_COLOUR_TEXTURE:
			    this->components = 4;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->width, this->height, 0, GL_RGBA, GL_FLOAT, NULL);
				break;
			case tz::graphics::TextureComponent::DEPTH_TEXTURE:
			    this->components = 1;
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, this->width, this->height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				break;
		}
		// Unbind the texture.
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

Texture::Texture(int width, int height, tz::graphics::TextureComponent texture_component): Texture(width, height, true, texture_component){}

Texture::Texture(std::string filename, bool mipmapping, bool gamma_corrected): texture_handle(0), width(0), height(0), components(0), gamma_corrected(gamma_corrected), bitmap({})
{
	unsigned char* imgdata = this->load_texture(filename.c_str());
	if(imgdata == nullptr)
	{
		std::cerr << "Texture from the path: '" << filename << "' could not be loaded.\n";
	}

	glGenTextures(1, &(this->texture_handle));
	// Let OGL know it's just a 2d texture.
	glBindTexture(GL_TEXTURE_2D, this->texture_handle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if(mipmapping)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, this->gamma_corrected ? GL_SRGB_ALPHA : GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgdata);
	if(mipmapping)
		glGenerateMipmap(GL_TEXTURE_2D);

	this->bitmap = Bitmap<PixelRGBA>();
	this->bitmap.pixels.reserve(static_cast<std::size_t>(std::abs(this->width * this->height)));
	for(std::size_t i = 3; i < std::abs(this->width * this->height); i += 4)
		this->bitmap.pixels.emplace_back(PixelRGBA{imgdata[i - 3], imgdata[i - 2], imgdata[i - 1], imgdata[i]});
	this->delete_texture(imgdata);
}

Texture::Texture(const Font& font, const std::string& text, SDL_Color foreground_colour, bool store_bitmap): Texture()
{
	if(font.font_handle == NULL)
		std::cerr << "Texture attempted to load from an invalid font. Error: \"" << TTF_GetError() << "\".\n";
	SDL_Surface* text_surface = TTF_RenderUTF8_Blended(font.font_handle, text.c_str(), foreground_colour);
	GLenum texture_format, bytes_per_pixel = text_surface->format->BytesPerPixel;
	constexpr long mask = 0x000000ff;
	this->width = text_surface->w;
	this->height = text_surface->h;
	if(bytes_per_pixel == 4) // alpha
	{
		if(text_surface->format->Rmask == mask)
			texture_format = GL_RGBA;
		else
			texture_format = GL_BGRA;
	}
	else
	{	// no alpha
		if(text_surface->format->Rmask == mask)
			texture_format = GL_RGB;
		else
			texture_format = GL_BGR;
	}
	glGenTextures(1, &(this->texture_handle));
	// Let OGL know it's just a 2d texture.
	glBindTexture(GL_TEXTURE_2D, this->texture_handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, bytes_per_pixel, text_surface->w, text_surface->h, 0, texture_format, GL_UNSIGNED_BYTE, text_surface->pixels);
	// if ctor parameter said to store the bitmap in RAM, then dewit. otherwise dont bother because it eats lots of ram and its in VRAM anyway
	if(store_bitmap)
	{
		auto pixel_data = reinterpret_cast<unsigned char*>(text_surface->pixels);
		this->bitmap = Bitmap<PixelRGBA>();
		this->bitmap.pixels.reserve(static_cast<std::size_t>(std::abs(this->width * this->height)));
		for(std::size_t i = 3; i < std::abs(this->width * this->height); i += 4)
			switch(texture_format)
			{
				case GL_RGBA:
				default:
					this->bitmap.pixels.emplace_back(PixelRGBA{pixel_data[i - 3], pixel_data[i - 2], pixel_data[i - 1], pixel_data[i]});
					break;
				case GL_BGRA:
					this->bitmap.pixels.emplace_back(PixelRGBA{pixel_data[i - 1], pixel_data[i - 2], pixel_data[i - 3], pixel_data[i]});
					break;
				case GL_RGB:
					this->bitmap.pixels.emplace_back(PixelRGBA{pixel_data[i - 3], pixel_data[i - 2], pixel_data[i - 1], 255});
					break;
				case GL_BGR:
					this->bitmap.pixels.emplace_back(PixelRGBA{pixel_data[i - 1], pixel_data[i - 2], pixel_data[i - 3], 255});
					break;
			}
	}
	SDL_FreeSurface(text_surface);
}

Texture::Texture(const Texture& copy): Texture(copy.width, copy.height, copy.get_texture_component())
{
	this->components = copy.components;
	this->bitmap = copy.bitmap;
	glCopyImageSubData(copy.texture_handle, GL_TEXTURE_2D, 0, 0, 0, 0, this->texture_handle, GL_TEXTURE_2D, 0, 0, 0, 0, copy.width, copy.height, 1);
}

Texture::Texture(Texture&& move): texture_handle(move.texture_handle), width(move.width), height(move.height), components(move.components)
{
	move.texture_handle = 0;
}

Texture::~Texture()
{
	// glDeleteTextures silently ignores 0 as a texture_handle so this should not cause problems if just moved. also due to this reason doesnt crash if the texture used the default constructor
	glDeleteTextures(1, &(this->texture_handle));
}

Texture& Texture::operator=(Texture&& rhs)
{
	glDeleteTextures(1, &(this->texture_handle));
	this->texture_handle = rhs.texture_handle;
	this->width = rhs.width;
	this->height = rhs.height;
	this->components = rhs.components;
	this->texture_component = rhs.texture_component;
	this->gamma_corrected = rhs.gamma_corrected;
	this->bitmap = rhs.bitmap;
	rhs.texture_handle = 0;
	return *this;
}

void Texture::bind(Shader* shader, unsigned int id, const std::string& sampler_name) const
{
	this->bind_with_string(shader, id, sampler_name);
}

int Texture::get_width() const
{
	return this->width;
}

int Texture::get_height() const
{
	return this->height;
}

tz::graphics::MipmapType Texture::get_mipmap_type() const
{
	GLint flag;
	glBindTexture(GL_TEXTURE_2D, this->texture_handle);
	glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &flag);
	glBindTexture(GL_TEXTURE_2D, 0);
	switch(flag)
	{
		case static_cast<GLint>(tz::graphics::MipmapType::NEAREST):
			return tz::graphics::MipmapType::NEAREST;
		case static_cast<GLint>(tz::graphics::MipmapType::LINEAR):
			return tz::graphics::MipmapType::LINEAR;
		case static_cast<GLint>(tz::graphics::MipmapType::NEAREST_MULTIPLE):
			return tz::graphics::MipmapType::NEAREST_MULTIPLE;
		case static_cast<GLint>(tz::graphics::MipmapType::LINEAR_MULTIPLE):
			return tz::graphics::MipmapType::LINEAR_MULTIPLE;
		default:
			return tz::graphics::MipmapType::NONE;
	}
}

bool Texture::has_mipmap() const
{	
	return this->get_mipmap_type() != tz::graphics::MipmapType::NONE;
}

const Bitmap<PixelRGBA>& Texture::get_bitmap() const
{
	return this->bitmap;
	/*
	if(this->bitmap.has_value())
		return this->bitmap.value();
	else
	{
		std::vector<PixelRGBA> pixels;
		std::vector<GLubyte> pixel_data;
		pixel_data.resize(this->components * this->width * this->height);
		pixels.reserve(pixel_data.size());
        glGetTextureImage(this->texture_handle, 0, GL_RGBA, GL_UNSIGNED_BYTE, sizeof(GLubyte) * pixel_data.size(), pixel_data.data());
        for(std::size_t i = 0; i < pixel_data.size(); i += 4)
			pixels.emplace_back(pixel_data[i], pixel_data[i + 1], pixel_data[i + 2], pixel_data[i + 3]);
		return {pixels, this->width, this->height};
	}
	*/
}

tz::graphics::TextureComponent Texture::get_texture_component() const
{
	return this->texture_component;
}

bool Texture::operator==(const Texture& rhs) const
{
	return this->texture_handle == rhs.texture_handle;
}

unsigned char* Texture::load_texture(const char* file_name)
{
	return stbi_load(file_name, &(this->width), &(this->height), &(this->components), 4);
}

// Deleting texture as far as stb_image is concerned (We want to leave it alone when it's gone to the GPU)
void Texture::delete_texture(unsigned char* imgdata)
{
	stbi_image_free(imgdata);
}

void Texture::bind_with_string(Shader* shader, unsigned int id, const std::string& sampler_uniform_name) const
{
	if(id > 31)
	{
		std::cerr << "FrameBuffer bind ID " << id << " is invalid. Must be between 1-31\n";
		return;
	}
	// this sets which texture we want to bind (id can be from 0 to 31)
	// GLTEXTURE0 is actually a number, so we can add the id instead of a massive switch statement
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_2D, this->texture_handle);
	shader->set_uniform<int>(sampler_uniform_name, id);
}

NormalMap::NormalMap(std::string filename): Texture(filename, false, false){}
NormalMap::NormalMap(): Texture(Bitmap<PixelRGBA>({tz::graphics::default_normal_map_pixel}, 1, 1)){}

void NormalMap::bind(Shader* shader, unsigned int id, const std::string& sampler_name) const
{
	this->bind_with_string(shader, id, sampler_name);
	shader->set_uniform<bool>("has_normal_map", true);
}

ParallaxMap::ParallaxMap(std::string filename, float multiplier, float offset): Texture(filename, false, false), multiplier(multiplier), bias(this->multiplier / 2.0f * offset){}
ParallaxMap::ParallaxMap(): Texture(Bitmap<PixelRGBA>({tz::graphics::default_parallax_map_pixel}, 1, 1)), multiplier(tz::graphics::asset::default_parallax_map_scale), bias(this->multiplier / 2.0f * (tz::graphics::asset::default_parallax_map_offset)){}

void ParallaxMap::bind(Shader* shader, unsigned int id, const std::string& sampler_name) const
{
    this->bind_with_string(shader, id, sampler_name);
	shader->set_uniform<bool>("has_parallax_map", true);
    shader->set_uniform<float>("parallax_multiplier", this->multiplier);
    shader->set_uniform<float>("parallax_bias", this->bias);
}

DisplacementMap::DisplacementMap(std::string filename, float displacement_factor): Texture(filename, false, false), displacement_factor(displacement_factor){}
DisplacementMap::DisplacementMap(): Texture(Bitmap<PixelRGBA>({tz::graphics::default_displacement_map_pixel}, 1, 1)), displacement_factor(tz::graphics::asset::default_displacement_factor){}

void DisplacementMap::bind(Shader* shader, unsigned int id, const std::string& sampler_name) const
{
    this->bind_with_string(shader, id, sampler_name);
	shader->set_uniform<bool>("has_displacement_map", true);
    shader->set_uniform<float>("displacement_factor", this->displacement_factor);
}

CubeMap::CubeMap(std::string right_texture, std::string left_texture, std::string top_texture, std::string bottom_texture, std::string back_texture, std::string front_texture): right_texture(std::move(right_texture)), left_texture(std::move(left_texture)), top_texture(std::move(top_texture)), bottom_texture(std::move(bottom_texture)), back_texture(std::move(back_texture)), front_texture(std::move(front_texture))
{
	glGenTextures(1, &(this->texture_handle));
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture_handle);
	std::vector<unsigned char*> face_data = this->load_textures();
	for(GLuint i = 0; i < face_data.size(); i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, this->width[static_cast<unsigned int>(i)], this->height[static_cast<unsigned int>(i)], 0, GL_RGBA, GL_UNSIGNED_BYTE, face_data[i]);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	for(auto& data : face_data)
		stbi_image_free(data);
}

CubeMap::CubeMap(std::string texture_directory, std::string skybox_name, std::string skybox_image_file_extension): CubeMap(texture_directory + skybox_name + "_rt" + skybox_image_file_extension, texture_directory + skybox_name + "_lf" + skybox_image_file_extension, texture_directory + skybox_name + "_up" + skybox_image_file_extension, texture_directory + skybox_name + "_dn" + skybox_image_file_extension, texture_directory + skybox_name + "_bk" + skybox_image_file_extension, texture_directory + skybox_name + "_ft" + skybox_image_file_extension){}

CubeMap::CubeMap(const CubeMap& copy): CubeMap(copy.right_texture, copy.left_texture, copy.top_texture, copy.bottom_texture, copy.back_texture, copy.front_texture){}

CubeMap::CubeMap(CubeMap&& move): texture_handle(move.texture_handle), right_texture(move.right_texture), left_texture(move.left_texture), top_texture(move.top_texture), bottom_texture(move.bottom_texture), back_texture(move.back_texture), front_texture(move.front_texture)
{
	for(unsigned int i = 0; i < 6; i++)
	{
		this->width[i] = move.width[i];
		this->height[i] = move.height[i];
		this->components[i] = move.components[i];
	}
	move.texture_handle = 0;
}

CubeMap::~CubeMap()
{
	glDeleteTextures(1, &(this->texture_handle));
}

void CubeMap::bind(Shader* shader, unsigned int id) const
{
	if(id > 31)
	{
		std::cerr << "FrameBuffer bind ID " << id << " is invalid. Must be between 1-31.\n";
		return;
	}
	// this sets which texture we want to bind (id can be from 0 to 31)
	// GLTEXTURE0 is actually a number, so we can add the id instead of a massive switch statement
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, this->texture_handle);
	shader->set_uniform<int>("cube_map_sampler", id);
}

std::vector<unsigned char*> CubeMap::load_textures()
{
	std::vector<unsigned char*> image_data;
	image_data.reserve(6);
	image_data.push_back(stbi_load((this->right_texture).c_str(), &(this->width[0]), &(this->height[0]), &(this->components[0]), 4));
	image_data.push_back(stbi_load((this->left_texture).c_str(), &(this->width[1]), &(this->height[1]), &(this->components[1]), 4));
	image_data.push_back(stbi_load((this->top_texture).c_str(), &(this->width[2]), &(this->height[2]), &(this->components[2]), 4));
	image_data.push_back(stbi_load((this->bottom_texture).c_str(), &(this->width[3]), &(this->height[3]), &(this->components[3]), 4));
	image_data.push_back(stbi_load((this->back_texture).c_str(), &(this->width[4]), &(this->height[4]), &(this->components[4]), 4));
	image_data.push_back(stbi_load((this->front_texture).c_str(), &(this->width[5]), &(this->height[5]), &(this->components[5]), 4));
	return image_data;
}