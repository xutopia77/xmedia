#ifndef _h264_parameter_h_
#define _h264_parameter_h_

// https://en.wikipedia.org/wiki/H.264/MPEG-4_AVC
enum h264_profile
{
	H264_PROFILE_UNKNOWN			= 0,
	H264_PROFILE_BASELINE			= 66,
	H264_PROFILE_MAIN				= 77,
	H264_PROFILE_EXTENDED			= 88,
	H264_PROFILE_HIGH				= 100,
	H264_PROFILE_HIGH10				= 110,
	H264_PROFILE_HIGH422			= 122,
	H264_PROFILE_HIGH444			= 244,
	H264_PROFILE_CAVLC				= 44,

	H264_PROFILE_SCALABLE_BASELINE	= 83,
	H264_PROFILE_SCALABLE_HIGH		= 86,

	H264_PROFILE_STEREO_HIGH		= 128,
	H264_PROFILE_MULTIVIEW_HIGH		= 118,
	H264_PROFILE_MULTIVIEW_DEPTH_HIGH = 138,
};

enum h264_level
{
	H264_LEVEL_UNKNOWN = 0,
	H264_LEVEL_1_0, // 128??96@30.9 (8) / 176??144@15.0 (4)
	H264_LEVEL_1_B, // 128??96@30.9 (8) / 176??144@15.0 (4)
	H264_LEVEL_1_1, // 176??144@30.3 (9) / 320??240@10.0 (3) / 352??288@7.5 (2)
	H264_LEVEL_1_2, // 320??240@20.0 (7) / 352??288@15.2 (6)
	H264_LEVEL_1_3, // 320??240@36.0 (7) / 352??288@30.0 (6)
	H264_LEVEL_2_0, // 320??240@36.0 (7) / 352??288@30.0 (6)
	H264_LEVEL_2_1, // 352??480@30.0 (7) / 352??576@25.0 (6)
	H264_LEVEL_2_2, // 352??480@30.7 (12) / 352??576@25.6 (10) / 720??480@15.0 (6) / 720??576@12.5 (5)
	H264_LEVEL_3_0, // 352??480@61.4 (12) / 352??576@51.1 (10) / 720??480@30.0 (6) / 720??576@25.0 (5)
	H264_LEVEL_3_1, // 720??480@80.0 (13) / 720??576@66.7 (11) / 1280??720@30.0 (5)
	H264_LEVEL_3_2, // 1280??720@60.0 (5) / 1280??1024@42.2 (4)
	H264_LEVEL_4_0, // 1280??720@68.3 (9) / 1920??1080@30.1 (4) / 2048??1024@30.0 (4)
	H264_LEVEL_4_1, // 1280??720@68.3 (9) / 1920??1080@30.1 (4) / 2048??1024@30.0 (4)
	H264_LEVEL_4_2, // 1280??720@145.1 (9) / 1920??1080@64.0 (4) / 2048??1080@60.0 (4)
	H264_LEVEL_5_0, // 1920??1080@72.3 (13) / 2048??1024@72.0 (13) / 2048??1080@67.8 (12) / 2560??1920@30.7 (5) / 3672??1536@26.7 (5)
	H264_LEVEL_5_1, // 1920??1080@120.5 (16) / 2560??1920@51.2 (9) / 3840??2160@31.7 (5) / 4096??2048@30.0 (5) / 4096??2160@28.5 (5) / 4096??2304@26.7 (5)
	H264_LEVEL_5_2, // 1920??1080@172.0 (16) / 2560??1920@108.0 (9) / 3840??2160@66.8 (5) / 4096??2048@63.3 (5) / 4096??2160@60.0 (5) / 4096??2304@56.3 (5)
};

struct h264_parameter_t
{
	int profile;		// H264_PROFILE_XXX
	int level;			// H264_LEVEL_XXX

	int width;
	int height;
	int format;			// PICTURE_YUV420/PICTURE_YUV422/PICTURE_YUV444

	int frame_rate;		// 25000/1000.0 => 25.0fps
	int gop_size;		// frames per gop
	int bitrate;		// bits per second
	int bitrate_mode;	// CBR/VBR/ABR/CQP/CRF

	int stream_mode;	// 0-H.264 AnnexB, 1-H.264 MP4
};

#endif /* !_h264_parameter_h_ */
