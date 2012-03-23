// F1*1.normal_pos = 0
// F1*2.normal_pos = 181.0
// F1*3.normal_pos = 362.0
// F1*4.normal_pos = 540.0
// F2*1.normal_pos = 0
// F2*2.normal_pos = 392.0
// F2*3.normal_pos = 693.0
// F2*4.normal_pos = 990.0
// F1X1.axial_shift = -0.00188482
// F1Y1.axial_shift = -0.369185
// F1X2.axial_shift = 0.602957
// F1Y2.axial_shift = 0.706791
// F1X3.axial_shift = -1.1753
// F1Y3.axial_shift = -0.421003
// F1X4.axial_shift = -0.0729813
// F1Y4.axial_shift = 0.174204
// F2X1.axial_shift = -2.15873
// F2Y1.axial_shift = 0.273533
// F2X2.axial_shift = 1.34369
// F2Y2.axial_shift = 0.482342
// F2X3.axial_shift = 0.791465
// F2Y3.axial_shift = -1.47704
// F2X4.axial_shift = 2.07286
// F2Y4.axial_shift = 0.990694
struct Geom {
    int plane;	// 0-1X, 1-1Y, 2-2X, 3-2Y etc
    int pitch;  // signed, mm
    int shift;  // mm
} geom[] = {	// array index -- logical board number
    {-1, 0, 0}, // No Lboard 0
// Proportional chambers 1F
// F1X1
    {0, 1, -100},	//1
    {0, 1, 0},		//2
// F1Y1
    {1, 1, -100},	//3
    {1, 1, 0},		//4
// F1X2 
    {2, -1, 100},	//5
    {2, -1, 0},		//6
// F1Y2 
    {3, -1, 100},	//7
    {3, -1, 0},		//8
// F1X3
    {4, -1, 100},	//9
    {4, -1, 0},		//10
// F1Y3
    {5, -1, 100},	//11
    {5, -1, 0},		//12
// F1X4
    {6, -1, 100},	//13
    {6, -1, 0},		//14
// F1Y4
    {7, -1, 100},	//15
    {7, -1, 0},		//16
// Proportional chambers 2F
// F2X1
    {8, -1, 100},	//17
    {8, -1, 0},		//18
// F2Y1
    {9, 1, -100},	//19
    {9, 1, 0},		//20
// F2X2
    {10, -1, 100}, 	//21
    {10, -1, 0},	//22
// F2Y2
    {11, 1, -100},	//23
    {11, 1, 0},		//24
// F2X3
    {12, -1, 100},	//25
    {12, -1, 0},	//26
// F2Y3
    {13, 1, -100},	//27
    {13, 1, 0},		//28
// F2X4
    {14, -1, 100},	//29
    {14, -1, 0},	//30
// F2Y4
    {15, 1, -100},	//31
    {15, 1, 0}, 	//32
//
    {-1, 0, 0}, // No Lboard 33
    {-1, 0, 0}, // No Lboard 34
    {-1, 0, 0}, // No Lboard 35
    {-1, 0, 0}, // No Lboard 36
    {-1, 0, 0}, // No Lboard 37
    {-1, 0, 0}, // No Lboard 38
    {-1, 0, 0}, // No Lboard 39
    {-1, 0, 0}, // No Lboard 40
    {-1, 0, 0}, // No Lboard 41
    {-1, 0, 0}, // No Lboard 42
    {-1, 0, 0}, // No Lboard 43
    {-1, 0, 0}, // No Lboard 44
    {-1, 0, 0}, // No Lboard 45
    {-1, 0, 0}, // No Lboard 46
    {-1, 0, 0}, // No Lboard 47
    {-1, 0, 0}, // No Lboard 48
    {-1, 0, 0}, // No Lboard 49
    {-1, 0, 0}, // No Lboard 50
    {-1, 0, 0}, // No Lboard 51
    {-1, 0, 0}, // No Lboard 52
    {-1, 0, 0}, // No Lboard 53
    {-1, 0, 0}, // No Lboard 54
    {-1, 0, 0}, // No Lboard 55
    {-1, 0, 0}, // No Lboard 56
    {-1, 0, 0}, // No Lboard 57
    {-1, 0, 0}, // No Lboard 58
    {-1, 0, 0}, // No Lboard 59
    {-1, 0, 0}, // No Lboard 60
    {-1, 0, 0}, // No Lboard 61
    {-1, 0, 0}, // No Lboard 62
    {-1, 0, 0}, // No Lboard 63
    {-1, 0, 0}, // No Lboard 64
    {-1, 0, 0}, // No Lboard 65
    {-1, 0, 0}, // No Lboard 66
    {-1, 0, 0}, // No Lboard 67
    {-1, 0, 0}, // No Lboard 68
    {-1, 0, 0}, // No Lboard 69
    {-1, 0, 0}, // No Lboard 70
    {-1, 0, 0}, // No Lboard 71
    {-1, 0, 0}, // No Lboard 72
    {-1, 0, 0}, // No Lboard 73
    {-1, 0, 0}, // No Lboard 74
    {-1, 0, 0}, // No Lboard 75
    {-1, 0, 0}, // No Lboard 76
    {-1, 0, 0}, // No Lboard 77
    {-1, 0, 0}, // No Lboard 78
    {-1, 0, 0}, // No Lboard 79
    {-1, 0, 0}, // No Lboard 80
    {-1, 0, 0}, // No Lboard 81
    {-1, 0, 0}, // No Lboard 82
    {-1, 0, 0}, // No Lboard 83
    {-1, 0, 0}, // No Lboard 84
    {-1, 0, 0}, // No Lboard 85
    {-1, 0, 0}, // No Lboard 86
    {-1, 0, 0}, // No Lboard 87
    {-1, 0, 0}, // No Lboard 88
    {-1, 0, 0}, // No Lboard 89
    {-1, 0, 0}, // No Lboard 90
    {-1, 0, 0}, // No Lboard 91
    {-1, 0, 0}, // No Lboard 92
    {-1, 0, 0}, // No Lboard 93
    {-1, 0, 0}, // No Lboard 94
    {-1, 0, 0}, // No Lboard 95
    {-1, 0, 0}, // No Lboard 96
    {-1, 0, 0}, // No Lboard 97
    {-1, 0, 0}, // No Lboard 98
    {-1, 0, 0}, // No Lboard 99
    {-1, 0, 0}, // No Lboard 100
//
// Drifrt Chambers LEFT
// Drift 1 LeftY (small Y plane 1)
    {20, -2, 24},       // 101
// Drift 1 LeftY (small Y plane 2)
    {21, -2, 25},       // 102
//
    {-1, 0, 0}, // No Lboard 103
    {-1, 0, 0}, // No Lboard 104
    {-1, 0, 0}, // No Lboard 105
    {-1, 0, 0}, // No Lboard 106
    {-1, 0, 0}, // No Lboard 107
    {-1, 0, 0}, // No Lboard 108
    {-1, 0, 0}, // No Lboard 109
    {-1, 0, 0}, // No Lboard 110
//
// Drift 2 LeftX (large X plane 1)
    {22, 2, -72},       // 111
    {22, 2, -24},       // 112
    {22, 2, 24},        // 113
// Drift 2 LeftX (large X plane 2)
    {23, 2, -71},       // 114
    {23, 2, -23},       // 115
    {23, 2, 25},        // 116
//
    {-1, 0, 0}, // No Lboard 117
    {-1, 0, 0}, // No Lboard 118
    {-1, 0, 0}, // No Lboard 119
    {-1, 0, 0}, // No Lboard 120
//
// Drift 3 LeftY (large Y plane 1)
    {24, 2, -48},       // 121
//    {25, 2, -47},       // 121, swapped with 123
    {24, 2, 0},         // 122
// Drift 3 LeftY (large Y plane 2)
    {25, 2, -47},       // 123
//    {24, 2, -48},       // 123, swapped with 121
    {25, 2, 1},         // 124
//
    {-1, 0, 0}, // No Lboard 125
    {-1, 0, 0}, // No Lboard 126
    {-1, 0, 0}, // No Lboard 127
    {-1, 0, 0}, // No Lboard 128
    {-1, 0, 0}, // No Lboard 129
    {-1, 0, 0}, // No Lboard 130
//
// Drift 4 LeftX (large X plane 1)
    {26, -2, 72},       // 131
    {26, -2, 24},       // 132
    {26, -2, -24},      // 133
// Drift 4 LeftX (large X plane 2)
    {27, -2, 73},       // 134
    {27, -2, 25},       // 135
    {27, -2, -23},      // 136
//
    {-1, 0, 0}, // No Lboard 137
    {-1, 0, 0}, // No Lboard 138
    {-1, 0, 0}, // No Lboard 139
    {-1, 0, 0}, // No Lboard 140
//

// Drifrt Chambers RIGHT
// Drift 5 RightY (small Y plane 1)
    {30, 2, -24},       // 141
// Drift 5 RightY (small Y plane 2)
    {31, 2, -23},       // 142
//
    {-1, 0, 0}, // No Lboard 143
    {-1, 0, 0}, // No Lboard 144
    {-1, 0, 0}, // No Lboard 145
    {-1, 0, 0}, // No Lboard 146
    {-1, 0, 0}, // No Lboard 147
    {-1, 0, 0}, // No Lboard 148
    {-1, 0, 0}, // No Lboard 149
    {-1, 0, 0}, // No Lboard 150
//
// Drift 6 RightX (large X plane 1)
    {32, 2, -72},       // 151
    {32, 2, -24},       // 152
    {32, 2, 24},        // 153
// Drift 6 RightX (large X plane 2)
    {33, 2, -71},       // 154
    {33, 2, -23},       // 155
    {33, 2, 25},        // 156
//
    {-1, 0, 0}, // No Lboard 157
    {-1, 0, 0}, // No Lboard 158
    {-1, 0, 0}, // No Lboard 159
    {-1, 0, 0}, // No Lboard 160
//
// Drift 7 RightY (large Y plane 1)
    {34, 2, -48},       // 161
    {34, 2, 0},         // 162
// Drift 7 RightY (large Y plane 2)
    {35, 2, -47},       // 163
    {35, 2, 1},         // 164
//
    {-1, 0, 0}, // No Lboard 165
    {-1, 0, 0}, // No Lboard 166
    {-1, 0, 0}, // No Lboard 167
    {-1, 0, 0}, // No Lboard 168
    {-1, 0, 0}, // No Lboard 169
    {-1, 0, 0}, // No Lboard 170
//
// Drift 8 RightX (large X plane 1)
    {36, -2, 72},       // 171
    {36, -2, 24},       // 172
    {36, -2, -24},      // 173
// Drift 8 RightX (large X plane 2)
    {37, -2, 73},       // 174
    {37, -2, 25},       // 175
    {37, -2, -23},      // 176
//
    {-1, 0, 0}, // No Lboard 177
    {-1, 0, 0}, // No Lboard 178
    {-1, 0, 0}, // No Lboard 179
    {-1, 0, 0}, // No Lboard 180
//
// Test Drift  (small X plane 1)
    {40, 2, -24},       // 181 (?? direction)
// Test Drift  (small X plane 2)
    {41, 2, -23},       // 182 (?? direction)
//
    {-1, 0, 0} // No Lboard 183
};
