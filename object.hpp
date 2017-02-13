#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "quaternion.hpp"
#include "mesh.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include "camera.hpp"

class Object
{
public:
	//Object(Mesh mesh, Texture texture, Vector3F pos, Vector3F rot, Vector3F scale);
	Object(std::string meshLink, std::string textureLink, Vector3F pos, Vector3F rot, Vector3F scale);
	Vector3F getPos();
	Vector3F getRot();
	Vector3F getScale();
	
	Vector3F& getPosR();
	Vector3F& getRotR();
	Vector3F& getScaleR();
	
	std::string getMeshLink();
	std::string getTextureLink();
	
	void render(Mesh* mesh, Texture* tex, Camera& cam, Shader& shad, float width, float height);
private:
	float m[16], v[16], p[16];
	Vector3F pos, rot, scale;
	std::string meshLink, textureLink;
	/*
	Mesh mesh;
	Texture tex;
	*/
};

#endif