#pragma once

#include "VertexBuffers/DefaultInstanceBuffer.h"
#include "Texture.h"

namespace VGF
{
	struct InstanceData
	{
		std::vector<std::byte> data = {};
		size_t count = 1;
		size_t stride = sizeof(DefaultInstance);

		InstanceData(const void* data, size_t count, size_t stride)
			: data(static_cast<const std::byte*>(data), static_cast<const std::byte*>(data) + count * stride)
			, count(count)
			, stride(stride)
		{
		}

		void Append(const void* data, size_t count, size_t stride)
		{
			this->stride = stride;

			const std::byte* src = static_cast<const std::byte*>(data);
			this->data.insert(this->data.end(), src, src + count * stride);
			count += count;
		}

		const void* Data() { return data.data(); }
	};

	class Shader;
	class RenderData
	{
	public:
		RenderData(const Shader& shader) : shader(&shader) {};
		~RenderData();

		template<typename T> RenderData& Set(std::string name, const T& v)
		{
			static_assert(std::is_trivially_copyable_v<T>, "Set() needs a trivially copyable type");
			static_assert(!std::is_pointer_v<T>, "Use Set(name, ptr, count) for arrays");

			if constexpr (std::is_same_v<T, bool>)
			{
				uint32_t u = v ? 1u : 0u;
				return SetBytes(name, &u, sizeof(u));
			}
			else return SetBytes(name, &v, sizeof(T));
		}
	
		template<typename T> RenderData& Set(std::string name, const std::vector<T>& v)
		{
			return SetBytes(name, v.data(), v.size() * sizeof(T));
		}

		template<typename T> RenderData& Set(std::string name, const T* p, size_t count)
		{
			return SetBytes(name, p, count * sizeof(T));
		}

		RenderData& Set(std::string name, const Texture& texture)
		{
			textures.insert(std::make_pair(name, &texture));
			return *this;
		}

		RenderData& Set(std::string name, const Texture* texture)
		{
			textures.insert(std::make_pair(name, texture));
			return *this;
		}

		struct Pending { std::string name; std::vector<std::byte> bytes; };

		DefaultInstance instance;
		InstanceData instanceData = InstanceData(&instance, 1, sizeof(DefaultInstance));

		std::unordered_map<std::string, Pending> pending;
		std::unordered_map<std::string, const Texture*> textures;

	private:
		const Shader* shader;

		RenderData& SetBytes(const std::string& n, const void* p, size_t sz) {
			Pending e;
			e.name.assign(n);
			e.bytes.assign(static_cast<const std::byte*>(p), static_cast<const std::byte*>(p) + sz);
			pending[e.name] = e;

			return *this;
		}
	};
}
