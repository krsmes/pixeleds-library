#pragma once 
#include "pixeleds-library.h"

namespace Hue {
    constexpr double RED          = 0.0;     // 0°
    constexpr double ORANGE       = 0.083;   // 30°
    constexpr double YELLOW       = 0.167;   // 60°
    constexpr double YELLOW_GREEN = 0.25;    // 90°
    constexpr double GREEN        = 0.333;   // 120°
    constexpr double BLUE_GREEN   = 0.417;   // 150°
    constexpr double CYAN         = 0.5;     // 180°
    constexpr double AZURE        = 0.583;   // 210°
    constexpr double BLUE         = 0.667;   // 240°
    constexpr double VIOLET       = 0.75;    // 270°
    constexpr double MAGENTA      = 0.833;   // 300°
    constexpr double PINK         = 0.917;   // 330°
    constexpr double RED_MAX      = 1.0;     // 360°
}

namespace Color {
    // Basic Colors
    static const PixCol BLACK      = 0x000000;  // hsl(0, 0.0, 0.0);    hsv(0, 0.0, 0.0)
    static const PixCol WHITE      = 0xFFFFFF;  // hsl(0, 0.0, 1.0);    hsv(0, 0.0, 1.0)
    static const PixCol OFF        = BLACK;
    static const PixCol ON         = WHITE;
    static const PixCol R          = {255,0,0};  // hsl(0, 1.0, 0.50);   hsv(0, 1.0, 1.0)
    static const PixCol G          = {0,255,0};  // hsl(120, 1.0, 0.50); hsv(120, 1.0, 1.0)
    static const PixCol B          = {0,0,255};  // hsl(240, 1.0, 0.50); hsv(240, 1.0, 1.0)

    // White Colors
    static const PixCol WHITE_SMOK     = 0xB8B8B8;  // hsl(0, 0%, 72%);   hsv(0, 0%, 72%)
    static const PixCol GHOST_WHITE    = 0xBDBDFF;  // hsl(240, 100%, 74%); hsv(240, 25%, 75%)
    static const PixCol SNOW           = 0xBFBDBD;  // hsl(0, 100%, 74%);   hsv(0, 17%, 75%)
    static const PixCol MINT_CREAM     = 0xBBFFEF;  // hsl(150, 100%, 73%); hsv(150, 27%, 100%)
    static const PixCol IVORY          = 0xBFBFB4;  // hsl(60, 100%, 72%);  hsv(60, 25%, 75%)
    static const PixCol AZURE          = 0xB4FFFF;  // hsl(180, 100%, 72%); hsv(180, 29%, 100%)
    static const PixCol HONEYDEW       = 0xB4FFB4;  // hsl(120, 100%, 72%); hsv(120, 29%, 100%)
    static const PixCol ALICE_BLUE     = 0xB4BCFF;  // hsl(208, 100%, 72%); hsv(208, 29%, 100%)
    static const PixCol LAVENDER_BLUSH = 0xBFB4B8;  // hsl(340, 100%, 72%); hsv(340, 25%, 75%)
    static const PixCol MISTY_ROSE     = 0xBFABA7;  // hsl(6, 100%, 70%);   hsv(6, 30%, 75%)
    
    // Red Colors
    static const PixCol MAROON         = 0x500202;  // hsl(0, 0.59, 0.15);   hsv(0, 0.60, 0.31)  
    static const PixCol DARK_RED       = 0x8B0000;  // hsl(0, 1.0, 0.27);    hsv(0, 1.0, 0.55)
    static const PixCol INDIAN_RED     = 0xCD5C5C;  // hsl(0, 0.53, 0.58);   hsv(0, 0.56, 0.80)
    static const PixCol LIGHT_CORAL    = 0xF08080;  // hsl(0, 0.79, 0.72);   hsv(0, 0.47, 0.94)
    static const PixCol RED            = 0xFF0804;  // hsl(1, 1.0, 0.51);    hsv(1, 0.98, 1.0)
    static const PixCol SALMON         = 0xFA8072;  // hsl(6, 0.93, 0.71);   hsv(6, 0.52, 0.98)
    static const PixCol LIGHT_SALMON   = 0xFFA07A;  // hsl(17, 1.0, 0.74);   hsv(17, 0.52, 1.0)
    static const PixCol CRIMSON        = 0xDC143C;  // hsl(348, 0.83, 0.47); hsv(348, 0.93, 0.86)
    static PixPal REDS = PixPal::create({MAROON, DARK_RED, INDIAN_RED, LIGHT_CORAL, RED, SALMON, LIGHT_SALMON, CRIMSON});

    // Orange Colors
    static const PixCol TOMATO         = 0xFF6347;  // hsl(9, 1.0, 0.64);   hsv(9, 0.72, 1.0)
    static const PixCol ORANGE_RED     = 0xFF4500;  // hsl(16, 1.0, 0.50);  hsv(16, 1.0, 1.0)
    static const PixCol CORAL          = 0xFF7F50;  // hsl(16, 1.0, 0.66);  hsv(16, 0.68, 1.0)
    static const PixCol DARK_ORANGE    = 0xAF0C00;  
    static const PixCol ORANGE         = 0xFF3C00; 
    static const PixCol GOLD           = 0xFFD700;  // hsl(51, 1.0, 0.50);  hsv(51, 1.0, 1.0)
    static PixPal ORANGES = PixPal::create({TOMATO, ORANGE_RED, CORAL, DARK_ORANGE, ORANGE, GOLD});

    // Yellow Colors
    static const PixCol KHAKI            = 0xF0E68C;  // hsl(54, 0.77, 0.75); hsv(54, 0.41, 0.94)
    static const PixCol PALE_GOLDEN_ROD  = 0xEEE8AA;  // hsl(55, 0.67, 0.82); hsv(55, 0.28, 0.93)
    static const PixCol DARK_KHAKI       = 0xBDB76B;  // hsl(56, 0.38, 0.58); hsv(56, 0.44, 0.74)
    static const PixCol YELLOW           = 0xFFFF00;  // hsl(60, 1.0, 0.50);  hsv(60, 1.0, 1.0)
    static const PixCol LIGHT_YELLOW     = 0xFFFFE0;  // hsl(60, 1.0, 0.94);  hsv(60, 0.125, 1.0)
    static PixPal YELLOWS = PixPal::create({KHAKI, PALE_GOLDEN_ROD, DARK_KHAKI, YELLOW, LIGHT_YELLOW});

    // Green Colors
    static const PixCol OLIVE             = 0x556B1B;  // hsl(73, 0.60, 0.26); hsv(73, 0.75, 0.42)
    static const PixCol GREEN_YELLOW      = 0xADFF2F;  // hsl(84, 1.0, 0.59);  hsv(84, 0.81, 1.0)
    static const PixCol LAWN_GREEN        = 0x7CFC00;  // hsl(90, 1.0, 0.49);  hsv(90, 1.0, 0.99)
    static const PixCol CHARTREUSE        = 0x7FFF00;  // hsl(90, 1.0, 0.50);  hsv(90, 1.0, 1.0)
    static const PixCol DARK_GREEN        = 0x002200; 
    static const PixCol GREEN             = 0x004000;  
    static const PixCol FOREST_GREEN      = 0x228B22;  // hsl(120, 0.61, 0.34); hsv(120, 0.75, 0.55)
    static const PixCol LIME_GREEN        = 0x32CD32;  // hsl(120, 0.61, 0.50); hsv(120, 0.75, 0.80)
    static const PixCol LIME              = 0x00FF00;  // hsl(120, 1.0, 0.50); hsv(120, 1.0, 1.0)
    static PixPal GREENS = PixPal::create({OLIVE, GREEN_YELLOW, LAWN_GREEN, CHARTREUSE, DARK_GREEN, GREEN, FOREST_GREEN, LIME_GREEN, LIME});
    
    // Cyan Colors
    static const PixCol TURQUOISE         = 0x40E0D0;  // hsl(174, 0.72, 0.56); hsv(174, 0.75, 0.88)
    static const PixCol MEDIUM_TURQUOISE  = 0x48D1CC;  // hsl(177, 0.60, 0.55); hsv(177, 0.64, 0.82)
    static const PixCol DARK_CYAN         = 0x008B8B;  // hsl(180, 1.0, 0.27); hsv(180, 1.0, 0.55)
    static const PixCol TEAL              = 0x008080;  // hsl(180, 1.0, 0.25); hsv(180, 1.0, 0.50)
    static const PixCol CYAN              = 0x00FFFF;  // hsl(180, 1.0, 0.50); hsv(180, 1.0, 1.0)
    static const PixCol DARK_TURQUOISE    = 0x00CED1;  // hsl(181, 1.0, 0.41); hsv(181, 1.0, 0.82)
    static PixPal CYANS = PixPal::create({TURQUOISE, MEDIUM_TURQUOISE, DARK_CYAN, TEAL, CYAN, DARK_TURQUOISE});

    // Blue Colors
    static const PixCol DEEP_SKY_BLUE     = 0x00BFFF;  // hsl(195, 1.0, 0.50); hsv(195, 1.0, 1.0)
    static const PixCol BLUE              = 0x0227AB;  
    static const PixCol DODGER_BLUE       = 0x1E90FF;  // hsl(210, 1.0, 0.56); hsv(210, 1.0, 1.0)
    static const PixCol ROYAL_BLUE        = 0x4169E1;  // hsl(225, 0.73, 0.57); hsv(225, 0.78, 0.88)
    static const PixCol NAVY              = 0x000080;  // hsl(240, 1.0, 0.25); hsv(240, 1.0, 0.50)
    static const PixCol DARK_BLUE         = 0x00008B;  // hsl(240, 1.0, 0.27); hsv(240, 1.0, 0.55)
    static const PixCol MEDIUM_BLUE       = 0x0000CD;  // hsl(240, 1.0, 0.40); hsv(240, 1.0, 0.80)
    static const PixCol MIDNIGHT_BLUE     = 0x191970;  // hsl(240, 0.64, 0.27); hsv(240, 0.83, 0.44)
    static const PixCol DARK_SLATE_BLUE   = 0x483D8B;  // hsl(248, 0.39, 0.39); hsv(248, 0.56, 0.55)
    static const PixCol SLATE_BLUE        = 0x6A5ACD;  // hsl(248, 0.53, 0.58); hsv(248, 0.57, 0.80)
    static const PixCol MEDIUM_SLATE_BLUE = 0x7B68EE;  // hsl(249, 0.80, 0.67); hsv(249, 0.57, 0.93)
    static PixPal BLUES = PixPal::create({DEEP_SKY_BLUE, BLUE, DODGER_BLUE, ROYAL_BLUE, NAVY, DARK_BLUE, MEDIUM_BLUE, MIDNIGHT_BLUE, DARK_SLATE_BLUE, SLATE_BLUE, MEDIUM_SLATE_BLUE});

    // Magenta/Purple Colors
    static const PixCol MEDIUM_PURPLE     = 0x9370DB;  // hsl(260, 0.60, 0.65); hsv(260, 0.43, 0.86)
    static const PixCol VIOLET            = 0x5A0FD2;  
    static const PixCol BLUE_VIOLET       = 0x2A1BE2; 
    static const PixCol MAGENTA           = 0xC010F0; 
    static const PixCol INDIGO            = 0x1B0052; 
    static const PixCol DARK_ORCHID       = 0x9932CC;  // hsl(280, 0.61, 0.50); hsv(280, 0.75, 0.80)
    static const PixCol DARK_MAGENTA      = 0x4B024B; 
    static const PixCol PURPLE            = 0x800080;  // hsl(300, 1.0, 0.25); hsv(300, 1.0, 0.50)
    static PixPal PURPLES = PixPal::create({MEDIUM_PURPLE, VIOLET, BLUE_VIOLET, MAGENTA, INDIGO, DARK_ORCHID, DARK_MAGENTA, PURPLE});

    // Other Color Sets
    static PixPal BW = PixPal::create({WHITE, BLACK});  // white is first so that blink/fade animations work

    static PixPal RGB = PixPal::create({R, G, B});
    static PixPal RYGB = PixPal::create({RED, YELLOW, GREEN, BLUE});
    static PixPal RYGB_STRIPES = PixPal::create({RED, 0, YELLOW, 0, GREEN, 0, BLUE, 0 });
    static PixPal CYM = PixPal::create({CYAN, YELLOW, MAGENTA});

    static PixPal RAINBOW = PixPal::create({RED, ORANGE, YELLOW, GREEN, BLUE, INDIGO, VIOLET});
/*
rainbow
0: 010000
1: 000700
2: 000001
3: 000000
4: 5ADA01
5: 2075E0
6: 0120CB
*/
    
}
