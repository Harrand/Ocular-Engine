#include "audio.hpp"

void tz::audio::initialise()
{
	constexpr int channels = 2; // number of sound chanels in output. 2 for stereo, 1 for mono.
	constexpr int chunk_size = 4096; // bytes used per output sample
	constexpr Uint16 format = MIX_DEFAULT_FORMAT; // output sample format. MIX_DEFAULT_FORMAT is the same as AUDIO_S16SYS (signed 16-bit samples, in system byte order)
	if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, format, channels, chunk_size) == -1)
		tz::util::log::error("Mix_OpenAudio returned -1: ", Mix_GetError(), "\n\tInitialisation of tz::audio failed.");
	else
		tz::util::log::message("Initialised tz::audio via SDL_Mixer.");
}

void tz::audio::terminate()
{
	Mix_CloseAudio();
	tz::util::log::message("Terminated tz::audio via SDL_Mixer.");
}

AudioClip::AudioClip(std::string filename): filename(std::move(filename))
{
	this->audio_handle = Mix_LoadWAV(this->filename.c_str());
}

AudioClip::AudioClip(const AudioClip& copy): AudioClip(copy.getFileName()){}

AudioClip::AudioClip(AudioClip&& move): filename(move.getFileName()), audio_handle(move.audio_handle)
{
	move.audio_handle = nullptr;
}

AudioClip::~AudioClip()
{
	// Cannot guarantee that Mix_FreeChunk(nullptr) doesn't lead to UB (this happens if this instance was moved to another) so put a check in here to prevent crashing
	if(this->audio_handle != nullptr)
		Mix_FreeChunk(this->audio_handle);
}

void AudioClip::play()
{
	constexpr int first_free_unreserved_channel = -1;
	constexpr int number_of_loops = 0;
	this->channel = Mix_PlayChannel(first_free_unreserved_channel, this->audio_handle, number_of_loops);
}

int AudioClip::getChannel() const
{
	return this->channel;
}

const std::string& AudioClip::getFileName() const
{
	return this->filename;
}

Mix_Chunk* AudioClip::getAudioHandle() const
{
	return this->audio_handle;
}

Mix_Chunk*& AudioClip::getAudioHandleR()
{
	return this->audio_handle;
}

AudioSource::AudioSource(std::string filename, Vector3F position): AudioClip(filename), position(std::move(position)){}

void AudioSource::update(const Camera& relative_to)
{
	const Vector3F source_position = this->position;
	const Vector3F listener_position = relative_to.getPosition();
	const Vector3F forward = relative_to.getForward();
	const Vector3F up = relative_to.getUp();
	float proportion_right = (forward.cross(up).normalised().dot((source_position - listener_position).normalised()) + 1) / 2;	//Get how strongly in the right that the audio source is at
	if(source_position == listener_position)
		proportion_right = 0.5;
	float right = proportion_right;
	float left = 1 - proportion_right;
	// Split stereo sound such that it reflects where the audio is relative to the player.
	Mix_SetPanning(this->getChannel(), left * 255, right * 255);
	float distance = (this->getPosition() - relative_to.getPosition()).length() / 100;
	// Attenuate audio of course
	Mix_Volume(this->getChannel(), 128 / (distance + 1));
}

const Vector3F& AudioSource::getPosition() const
{
	return this->position;
}

Vector3F& AudioSource::getPositionR()
{
	return this->position;
}

AudioMusic::AudioMusic(std::string filename): filename(std::move(filename)), paused(false)
{
	this->audio_handle = Mix_LoadMUS(this->filename.c_str());
}
AudioMusic::AudioMusic(const AudioMusic& copy): AudioMusic(copy.getFileName()){}

AudioMusic::AudioMusic(AudioMusic&& move): filename(move.getFileName()), audio_handle(move.audio_handle)
{
	move.audio_handle = nullptr;
}

AudioMusic::~AudioMusic()
{
	Mix_FreeMusic(this->audio_handle);
}

const std::string& AudioMusic::getFileName() const
{
	return this->filename;
}

Mix_Music* AudioMusic::getAudioHandle() const
{
	return this->audio_handle;
}

Mix_Music*& AudioMusic::getAudioHandleR()
{
	return this->audio_handle;
}

void AudioMusic::play(bool priority) const
{
	if(priority || Mix_PlayingMusic() != 0)
		Mix_PlayMusic(this->audio_handle, -1);
}

void AudioMusic::setPaused(bool pause)
{
	this->paused = pause;
	if(this->paused)
		Mix_PauseMusic();
	else
		Mix_ResumeMusic();
}

void AudioMusic::togglePaused()
{
	this->setPaused(!this->paused);
}