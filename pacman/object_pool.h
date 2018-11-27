#pragma once

#include <vector>

template <class T>
class ObjectPool
{
public:

	void Create(unsigned int num_objects/*, AvancezLib * system*/)
	{
		Deallocate();

		for (unsigned int i = 0; i < num_objects; i++)
		{
			T * t = new T();
			pool.push_back(t);
		}
	}

	void Destroy()
	{
		for (auto it = pool.begin(); it != pool.end(); it++)
			(*it)->Destroy();
	}

	void Deallocate()
	{
		SDL_Log("ObjectPool::Deallocating ");
		for (std::vector<T*>::iterator it = pool.begin(); it != pool.end(); it++)
			delete *it;
	}

	void DisableAll()
	{
		for (std::vector<T*>::iterator it = pool.begin(); it != pool.end(); it++)
		{
			(**it).enabled = false;
		}
	}

	~ObjectPool()
	{
		SDL_Log("ObjectPool::~ObjectPool");
		Deallocate();
	}

	T* FirstAvailable()
	{
		for (std::vector<T*>::iterator it = pool.begin(); it != pool.end(); it++)
			if (!(**it).enabled)
				return (*it);

		return NULL;
	}

	// sleect a random, enabled element in the object pool
	T* SelectRandom()
	{
		int offset = (rand() / (float)RAND_MAX) * pool.size();

		for (int i = 0; i < pool.size(); i++)
		{
			int index = (i + offset) % pool.size();

			if (pool[index]->enabled)
				return pool[index];
		}

		return NULL;
	}

	std::vector<T*> pool;
private:
};