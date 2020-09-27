#pragma once

class Color
{
public:
	Color() : r(0), g(0), b(0) {}
	Color(float r_, float g_, float b_) :
		r(r_), g(g_), b(b_) 
	{
		truncate();
	}
	inline Color operator*(float k);
	inline Color operator+(int k);
	inline Color operator+(const Color &c);
	inline bool operator==(Color &c);

	float r;
	float g;
	float b;

private:
	void truncate();
};

inline Color Color::operator*(float k)
{
	int newR = (float)r * k;
	int newG = (float)g * k;
	int newB = (float)b * k;

	return Color(newR, newG, newB);
}

inline Color Color::operator+(int k)
{
	int newR = (int)r + k;
	int newG = (int)g + k;
	int newB = (int)b + k;

	return Color(newR, newG, newB);
}

inline Color Color::operator+(const Color &c)
{
	unsigned int newR = r + c.r;
	unsigned int newG = g + c.g;
	unsigned int newB = b + c.b;

	return Color(newR, newG, newB);
}

inline bool Color::operator==(Color &c)
{
	if (this->r == c.r &&
		this->g == c.g &&
		this->b == c.b)
		return true;
	return false;
}

inline void Color::truncate()
{
	if (r > 1.0) r = 1.0;
	if (r < 0) r = 0;
	if (g > 1.0) g = 1.0;
	if (g < 0) g = 0;
	if (b > 1.0) b = 1.0;
	if (b < 0) b = 0;
}
