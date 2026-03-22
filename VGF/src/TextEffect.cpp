#include "TextEffect.h"

namespace VGF
{
    TextEffect TextEffect::Typewriter(double interval)
    {
        VGF::Timer timer = VGF::Timer(interval);
        std::string currentText = "";
        std::string builtText = "";
        unsigned int charIndex = 0;

        TextEffect effect;

        effect.runEffect = [=](const std::string& text) mutable -> std::string
        {
            if (text != currentText)
            {
                currentText = text;
                builtText = "";
                charIndex = 0;
            }
            if (timer.Tick(VGF::Time::GetDeltaTime()))
            {
                if (charIndex < text.size())
                {
                    builtText.push_back(text[charIndex]);
                    charIndex++;
                }
            }
            return builtText;
        };

        return effect;
    }
}