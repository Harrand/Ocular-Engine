#ifndef SCENE_HPP
#define SCENE_HPP
#include <map>
#include <cstddef>
//#include <functional>
#include "object.hpp"
#include "MDL/mdl_file.hpp"

namespace tz::scene
{
	constexpr char spawnpoint_tag_name[] = "spawnpoint";
	constexpr char spawnorientation_tag_name[] = "spawnorientation";
	constexpr char objects_sequence_name[] = "objects";

	constexpr char object_tag_prefix[] = "object";
}

/**
* Contains Objects, EntityObjects and pretty much any Topaz renderable you can think of.
* Use Scenes to store everything you want to draw.
*/
class Scene
{
public:
	/**
	* Construct an empty scene.
	*/
	Scene();
	/**
	* Load a scene from an existing MDL file. Takes in all asset vectors. Should probably be replaced with just a const asset manager reference of some kind to read the data without all this verbosity.
	*/
	Scene(std::string filename, std::string resources_path, const std::vector<std::unique_ptr<Mesh>>& all_meshes, const std::vector<std::unique_ptr<Texture>>& all_textures, const std::vector<std::unique_ptr<NormalMap>>& all_normal_maps, const std::vector<std::unique_ptr<ParallaxMap>>& all_parallax_maps, const std::vector<std::unique_ptr<DisplacementMap>>& all_displacement_maps, bool batch = true);
	Scene(const Scene& copy) = default;
	Scene(Scene&& move) = default;
	~Scene() = default;
	Scene& operator=(const Scene& rhs) = default;
	
	/**
	* Returns true if the scene was loaded from an external file.
	*/
	bool has_file_name() const;
	/**
	* Throws a std::bad_optional_access exception if this->has_file_name() returns false.
	*/
	const std::string& get_file_name() const;
	// Add a copy of an object to the scene. Complexity: O(1) amortised Ω(1) ϴ(1) amortised
	void add_object(Object obj);
	/**
	* Construct an Object3D, Entity or EntityObject3D in-place and add it to the scene.
	*/
	template<class Element, typename... Args>
	Element& emplace(Args&&... args);
	/**
	* Construct an Object3D in-place and add it to the scene.
	*/
	template<typename... Args>
	Object& emplace_object(Args&&... args);
	/**
	* Construct an Entity in-place and add it to the scene.
	*/
	/**
	* Remove an existing Object3D from the scene.
	*/
	void remove_object(const Object& obj);
	/**
	* Access all Object elements in the scene (Read-only).
	*/
	std::vector<std::reference_wrapper<const Object>> get_objects() const;
	/**
	* Returns total number of Object3Ds, Entities and EntityObject3Ds in the scene.
	*/
	std::size_t get_size() const;
	/**
	* Export scene data to a MDL file called scene_link
	* Complexity: O(n + m) Ω(1) ϴ(n + m), where n = number of objects and m = number of entity_objects.
	*/
	void export_scene(const std::string& scene_link) const;
	/**
	* Export scene data to a MDL file with the same name as the file which loaded this scene, overwriting it.
	* Complexity: See Scene::export_scene.
	*/
	void save() const;
	/**
	* Render all elements in the scene from the perspective of the camera, attaching a shader and updating uniforms in the process. 
	* This method should be invoked as often as possible, to smooth gameplay.
	* Complexity: O(n + p) Ω(1) ϴ(n + p), where n = number of objects, p = number of entity_objects.
	*/
	void render(const Camera& cam, Shader* shader, unsigned int width, unsigned int height);

	Vector3F spawn_point, spawn_orientation;
private:
	static Object retrieve_object_data(const std::string& object_name, const std::string& resources_path, MDLFile& mdlf, const std::vector<std::unique_ptr<Mesh>>& all_meshes, const std::vector<std::unique_ptr<Texture>>& all_textures, const std::vector<std::unique_ptr<NormalMap>>& all_normal_maps, const std::vector<std::unique_ptr<ParallaxMap>>& all_parallax_maps, const std::vector<std::unique_ptr<DisplacementMap>>& all_displacement_maps);

	std::optional<std::string> filename;
	std::optional<std::string> resources_path;
    /// Used to store Objects
	std::vector<Object> objects;
    /// Used to store Object sub-classes to prevent object slicing.
    /// Needs a shared-ptr because Scenes are copyable and SHOULD be copyable
    std::vector<std::shared_ptr<Object>> heap_objects;
};

#include "scene.inl"

#endif