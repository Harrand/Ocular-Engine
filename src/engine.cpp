#include "engine.hpp"
#include "graphics.hpp"
#include "data.hpp"

void tz::initialise()
{
	tz::util::log::message("Initialising Topaz...");
	SDL_Init(SDL_INIT_EVERYTHING);
	tz::util::log::message("Initialised SDL2.");
	tz::audio::initialise();
	tz::util::log::message("Initialised Topaz. Ready to receive OpenGL context...");
}

void tz::terminate()
{
	tz::util::log::message("Terminating Topaz...");
	tz::graphics::terminate();
	tz::audio::terminate();
	SDL_Quit();
	tz::util::log::message("Terminated SDL2.");
	tz::util::log::message("Terminated Topaz.");
}

Engine::Engine(Window& wnd, std::string properties_path, unsigned int initial_fps, unsigned int tps): properties(RawFile(properties_path)), resources(RawFile(this->properties.getTag("resources"))), default_shader(this->properties.getTag("default_shader")), default_gui_shader(this->properties.getTag("default_gui_shader")), camera(Camera()), wnd(wnd), world(this->properties.getTag("default_world"), this->properties.getTag("resources")), fps(initial_fps), tps(tps), command_executor(), update_due(false)
{
	this->camera.getPositionR() = this->world.getSpawnPoint();
	tz::data::Manager(this->properties.getTag("resources")).retrieveAllData(this->meshes, this->textures, this->normal_maps, this->parallax_maps, this->displacement_maps);
	for(std::string shader_path : this->properties.getSequence("extra_shaders"))
		this->extra_shaders.emplace_back(shader_path);
}

void Engine::update(std::size_t shader_index)
{
	static Timer ticker;
	if(this->keeper.millisPassed(1000))
	{
		this->fps = this->profiler.getFPS();
		this->profiler.reset();
		this->keeper.reload();
	}
	this->wnd.setRenderTarget();
	this->profiler.beginFrame();
	
	this->keeper.update();
	ticker.update();
	this->wnd.clear(0.0f, 0.0f, 0.0f, 1.0f);
	this->profiler.endFrame();
	
	this->world.render(this->camera, this->getShader(shader_index), this->wnd.getWidth(), this->wnd.getHeight(), this->meshes, this->textures, this->normal_maps, this->parallax_maps, this->displacement_maps);
	
	for(auto command : this->command_executor.getCommandsR())
		command->operator()({});
	
	if(ticker.millisPassed(1000.0f/this->tps))
	{
		this->world.update(this->tps);
		ticker.reload();
		this->update_due = true;
	}
	else
		this->update_due = false;
	this->wnd.update();
	
	GLenum error;
		if((error = glGetError()) != GL_NO_ERROR)
			tz::util::log::error("OpenGL Error: ", error, "\n");
}

const Timer& Engine::getTimer() const
{
	return this->keeper;
}

const TimeProfiler& Engine::getTimeProfiler() const
{
	return this->profiler;
}

const MDLF& Engine::getProperties() const
{
	return this->properties;
}

MDLF& Engine::getPropertiesR()
{
	return this->properties;
}

const MDLF& Engine::getResources() const
{
	return this->resources;
}

MDLF& Engine::getResourcesR()
{
	return this->resources;
}

const Camera& Engine::getCamera() const
{
	return this->camera;
}

const Window& Engine::getWindow() const
{
	 return this->wnd;
}

const World& Engine::getWorld() const
{
	return this->world;
}

const Shader& Engine::getDefaultShader() const
{
	return this->default_shader;
}

const Shader& Engine::getDefaultGuiShader() const
{
	return this->default_gui_shader;
}

Camera& Engine::getCameraR()
{
	return this->camera;
}

Window& Engine::getWindowR()
{
	return this->wnd;
}

World& Engine::getWorldR()
{
	return this->world;
}

const std::vector<std::unique_ptr<Mesh>>& Engine::getMeshes() const
{
	return this->meshes;
}

const std::vector<std::unique_ptr<Texture>>& Engine::getTextures() const
{
	return this->textures;
}

const std::vector<std::unique_ptr<NormalMap>>& Engine::getNormalMaps() const
{
	return this->normal_maps;
}

const std::vector<std::unique_ptr<ParallaxMap>>& Engine::getParallaxMaps() const
{
	return this->parallax_maps;
}

const std::vector<std::unique_ptr<DisplacementMap>>& Engine::getDisplacementMaps() const
{
	return this->displacement_maps;
}

const Shader& Engine::getShader(std::size_t index) const
{
	if(index > this->extra_shaders.size())
		tz::util::log::error("Could not retrieve shader index ", index, ", retrieving default instead.");
	else if(index != 0)
		return this->extra_shaders.at(index - 1);
	return this->getDefaultShader();
}

unsigned int Engine::getFPS() const
{
	return this->fps;
}

unsigned int Engine::getTPS() const
{
	return this->tps;
}

const CommandExecutor& Engine::getCommandExecutor() const
{
	return this->command_executor;
}

CommandExecutor& Engine::getCommandExecutorR()
{
	return this->command_executor;
}

bool Engine::isUpdateDue() const
{
	return this->update_due;
}