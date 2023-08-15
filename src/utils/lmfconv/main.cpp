#include "pch.h"

int print_help()
{
	puts(
		"USAGE: lmfconv.exe [options] <file name>" NL
		"OPTIONS:" NL
		"-fbx - conversion from fbx to lmf" NL
		"-dae - conversion from dae to lmf" NL
		"-tri - do triangulation [FBX only]" NL
		"-o   - specify output file name" NL
	);
	return (EXIT_SUCCESS);
}

uint32_t get_param(uint32_t argc, const char** argv, const char* param)
{
	uint32_t idx = 0;
	for (uint32_t i = 1; i < argc; ++i)
	{
		if (0 == _stricmp(argv[i], param))
		{
			idx = i;
			break;
		}
	}
	return (idx);
}

int main(uint32_t argc, const char** argv)
{
	int result = EXIT_SUCCESS;
	puts("LMF converter v0.5");

	if (argc > 1)
	{
		char outFileName_[_MAX_PATH];
		bool isFbxConv = !!get_param(argc, argv, "-fbx");
		bool isDaeConv = !!get_param(argc, argv, "-dae");
		const char* inFileName = argv[argc - 1];
		const char* outFileName = NULL;
		if (isFbxConv || isDaeConv)
		{
			if (isFbxConv)
				puts("FBX to LMF conversion mode");
			else
				puts("DAE to LMF conversion mode");

			uint32_t outFileNameIdx;
			if ((outFileNameIdx = get_param(argc, argv, "-o")) && (outFileNameIdx + 1) < argc)
			{
				outFileName = argv[outFileNameIdx + 1];
			}
			else
			{
				outFileName = outFileName_;

				const char* fileName = strrchr(inFileName, '\\');
				const char* fileExt = strrchr(inFileName, '.');

				if (fileName)
				{
					strncpy_s(outFileName_, inFileName, size_t(fileName - inFileName) + 1);
				}
				else
				{
					outFileName_[0] = '\0';
				}
				

				if (fileExt)
				{
					if (fileName)
						strncat_s(outFileName_, fileName + 1, size_t(fileExt - fileName) - 1);
					else
						strncat_s(outFileName_, inFileName, size_t(fileExt - inFileName));
				}
				else
				{
					if (fileName)
						strcat_s(outFileName_, fileName + 1);
					else
						strcat_s(outFileName_, inFileName);
				}
				strcat_s(outFileName_, OUT_EXTENSION);
			}

			print("Input file: %s" NL, inFileName);
			print("Output file: %s" NL, outFileName);

			bool bAllOk = false;
			ConversionOptions options = 0;
			if (isFbxConv)
			{
				if (get_param(argc, argv, "-tri")) options |= ConversionOptions_FBX_Triangulate;
				bAllOk = fbxconv_perform(inFileName, outFileName, options);
			}
			else
			{
				bAllOk = daeconv_perform(inFileName, outFileName, options);
			}
			result = bAllOk ? EXIT_SUCCESS : EXIT_FAILURE;
			if (bAllOk) print("Conversion succesfully completed!" NL);
		}
		else
		{
			result = print_help();
		}
	}
	else
	{
		result = print_help();
	}
	return (result);
}
