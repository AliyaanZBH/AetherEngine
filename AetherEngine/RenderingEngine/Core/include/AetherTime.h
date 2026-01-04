#pragma once
//===============================================================================
// desc: Simple namespace wrapper for easy access to time across engine and games
// auth: Aliyaan Zulfiqar
//===============================================================================
#include <chrono>
//===============================================================================

namespace Aether
{
	class AETHER_API Time
	{
	public:

		inline static void Update()
		{
			static auto startTimestamp = std::chrono::high_resolution_clock::now();
			auto now = std::chrono::high_resolution_clock::now();
			s_DeltaTime = std::chrono::duration<float>(now - startTimestamp).count();
			startTimestamp = now;
		}

		inline static float GetDT() { return s_DeltaTime; }
	private:
		inline static float s_DeltaTime = 0.f;
	};
};
