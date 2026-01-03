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
			static auto firstTimestamp = std::chrono::high_resolution_clock::now();
			auto now = std::chrono::high_resolution_clock::now();
			s_DeltaTime = std::chrono::duration<float>(now - firstTimestamp).count();
			firstTimestamp = now;
		}

		inline static float GetDT() { return s_DeltaTime; }
	private:
		inline static float s_DeltaTime = 0.f;
	};
};