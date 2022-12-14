#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"  
#include <math.h>

#define MAX_LEN_LIST 256

enum INLETS { I_INPUT, I_LIST, NUM_INLETS };
enum OUTLETS { O_OUTPUT, O_INDEX, NUM_OUTLETS };

typedef struct {

	t_object obj;
	
	float *list;
	long list_len;
	long max_len;

	void *m_outlet1;
	void *m_outlet2;

} t_fl_dele;

void *fl_dele_new(t_symbol *s, short argc, t_atom *argv);
void fl_dele_assist(t_fl_dele *x, void *b, long msg, long arg, char *dst);

void fl_dele_int(t_fl_dele *x, long n);
void fl_dele_float(t_fl_dele *x, double f);
void fl_dele_list(t_fl_dele *x, t_symbol *s, long argc, t_atom *argv);

void fl_dele_list_len(t_fl_dele *x, t_symbol *s, long argc, t_atom *argv);
void fl_dele_free(t_fl_dele *x);

void swap(float *px, float *py);

static t_class *fl_dele_class;