#pragma once
#include <vector>
#include <string>

#pragma pack(1)
struct NewBitMapHeader {
	int width;
	int height;
};

struct Color {
	unsigned char red;
	unsigned char green;
	unsigned char blue;
	unsigned char alpha;
};
#pragma pack()

class Texture final
{
private:
	std::unique_ptr<char[]> m_Buffer;
	int m_Width;
	int m_Height;
	int m_PixelByte;

public:
	Texture();
	Texture(const std::string& str);
	Texture(const Texture& texture) = default;
	Texture& operator=(const Texture& texture) = default;
	/* virtual */ ~Texture();

	int GetWidth(void) const;
	int GetHeight(void) const;
	int GetPixelByte(void) const;
	int GetSize(void) const;
	void LoadTexture(const std::string& str);
	const char* GetTextureBuffer(void) const;
};

