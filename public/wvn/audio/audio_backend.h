#pragma once

namespace wvn::sfx
{
	struct AudioProperties
	{
		bool temp;
	};

	class AudioBackend
	{
	public:
		AudioBackend() = default;
		virtual ~AudioBackend() = default;

		virtual AudioProperties properties() = 0;
	};
}
