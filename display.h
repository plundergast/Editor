#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <memory>

class display : public olc::PixelGameEngine
{
public:
	display ();
	~display ();
	std::shared_ptr <std::vector<std::string>> text_to_parse;
protected:
	bool OnUserCreate () override;
	bool OnUserUpdate (float deltaTime) override;
private:
	void input ();
	void render_text ();
};