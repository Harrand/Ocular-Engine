#ifndef TEXTURE_HPP
#define TEXTURE_HPP
#include <string>
#include <cassert>
#include <iostream>
#include <vector>
#include <memory>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include "glew.h"

class FrameBuffer
{
public:
	FrameBuffer(unsigned int width = 256, unsigned int height = 256);
	void setRenderTarget() const;
	void bind(unsigned int id) const;
private:
	unsigned int width, height;
	GLuint fbHandle, texHandle, depthRenderBufferHandle;
};

class Texture
{
public:
	Texture(std::string filename = "./res/textures/undefined.jpg");
	~Texture();
	void bind(GLuint shaderProgram, unsigned int id);
	std::string getFileName() const;
	static std::shared_ptr<Texture> getFromLink(std::string textureLink, std::vector<std::shared_ptr<Texture>> allTextures);
protected:
	GLuint textureID;
	GLuint texhandle;
private:
	unsigned char* loadTexture();
	void deleteTexture(unsigned char* imgdata);
	int width, height, comps;
	std::string filename;
};

class NormalMap: public Texture
{
public:
	NormalMap(std::string filename = "./res/normalmaps/undefined.jpg");
	void bind(GLuint shaderProgram, unsigned int id);
	static std::shared_ptr<NormalMap> getFromLink(std::string normalMapLink, std::vector<std::shared_ptr<NormalMap>> allNormalMaps);
private:
};

#endif