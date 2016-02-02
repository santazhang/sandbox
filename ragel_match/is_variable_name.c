
#line 1 "is_variable_name.rl"
#include <stdio.h>
#include <string.h>


#line 8 "is_variable_name.c"
static const char _is_variable_name_key_offsets[] = {
	0, 0, 5
};

static const char _is_variable_name_trans_keys[] = {
	95, 65, 90, 97, 122, 95, 48, 57, 
	65, 90, 97, 122, 0
};

static const char _is_variable_name_single_lengths[] = {
	0, 1, 1
};

static const char _is_variable_name_range_lengths[] = {
	0, 2, 3
};

static const char _is_variable_name_index_offsets[] = {
	0, 0, 4
};

static const char _is_variable_name_trans_targs[] = {
	2, 2, 2, 0, 2, 2, 2, 2, 
	0, 0
};

static const int is_variable_name_start = 1;
static const int is_variable_name_first_final = 2;
static const int is_variable_name_error = 0;

static const int is_variable_name_en_main = 1;


#line 10 "is_variable_name.rl"


int is_variable_name(const char* str) {
    const char* p = str;
    const char* pe = str + strlen(str);
    int cs;

    
#line 51 "is_variable_name.c"
	{
	cs = is_variable_name_start;
	}

#line 18 "is_variable_name.rl"
    
#line 58 "is_variable_name.c"
	{
	int _klen;
	unsigned int _trans;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_keys = _is_variable_name_trans_keys + _is_variable_name_key_offsets[cs];
	_trans = _is_variable_name_index_offsets[cs];

	_klen = _is_variable_name_single_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _is_variable_name_range_lengths[cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	cs = _is_variable_name_trans_targs[_trans];

	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 19 "is_variable_name.rl"

    if (cs == is_variable_name_first_final) {
        return 1;
    }
    return 0;
}

int main() {
    printf("%d\n", is_variable_name("abc"));
    printf("%d\n", is_variable_name("abc "));
    printf("%d\n", is_variable_name("0abc"));
    return 0;
};
