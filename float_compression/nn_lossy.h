#pragma once

void nn_compress(const float* farr, int n_floats, char** compressed, int* compressed_bytes);

void nn_decompress(const char* compressed, int compressed_bytes, float* farr, int n_floats);
