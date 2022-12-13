#include "fldele.h"

void C74_EXPORT main() {

	t_class *c;

	c = class_new("fldele",(method)fl_dele_new, (method)fl_dele_free, sizeof(t_fl_dele), 0, 0);
	class_addmethod(c, (method)fl_dele_assist,"assist", A_CANT, 0);
	class_addmethod(c,(method)fl_dele_int, "int", A_LONG, 0);
	class_addmethod(c,(method)fl_dele_float, "float", A_LONG, 0);
	class_addmethod(c,(method)fl_dele_list, "list", A_GIMME, 0);
	class_addmethod(c, (method)fl_dele_list_len, "list_len", A_GIMME, 0);

	class_register(CLASS_BOX, c);
	fl_dele_class = c; 
}

void *fl_dele_new(t_symbol *s, short argc, t_atom *argv) 
{
	t_fl_dele *x = (t_fl_dele *)object_alloc(fl_dele_class);

	inlet_new((t_object *)x, "list");
	x->m_outlet2 = intout((t_object *)x);
	x->m_outlet1 = floatout((t_object *)x);

	x->list_len = 0;
	x->max_len = MAX_LEN_LIST;
	x->list = (float *)sysmem_newptr(MAX_LEN_LIST * sizeof(float));
	
	if (!x->list) { object_error((t_object *)x, "no memory for array"); }
	else {
		for (long i = 0; i < MAX_LEN_LIST; i++) {
			x->list[i] = 0;
		}
	}
	
	return x;
}

void fl_dele_assist(t_fl_dele *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET) {										
		switch (arg) {
		case I_INPUT: sprintf(dst, "(float) evaluated number"); break;
		case I_LIST: sprintf(dst, "(list) starting interval numbers"); break;
		}

	}
	else if (msg == ASSIST_OUTLET) {    
		switch (arg) {
		case O_OUTPUT: sprintf(dst, "pass value"); break;
		case O_INDEX: sprintf(dst, "index"); break;
		}
	}
}

void fl_dele_float(t_fl_dele *x, double f) 
{
	if (f != f) { object_warn((t_object *)x, "wrong number"); return; }

	float val = (float)f;

	long list_len = x->list_len;
	float *list = x->list;
	long index = list_len + 1;

	for (long i = 0; i < list_len; i++) {
		if (val < x->list[i]) {
			index = i + 1;
			break;
		}
	}

	outlet_int(x->m_outlet2, index);
	outlet_float(x->m_outlet1, val);
}

void fl_dele_int(t_fl_dele *x, long n) 
{
	fl_dele_float(x, (float)n);
}

void fl_dele_list(t_fl_dele *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	long list_len = ac;

	long min_idx;

	if (ac > MAX_LEN_LIST) {
		object_warn((t_object *)x, "list length truncated to: %d", MAX_LEN_LIST);
		list_len = MAX_LEN_LIST;
	}

	for (long i = 0; i < list_len; i++) {
		x->list[i] = (float)atom_getfloat(ap + i);
	}

	//delete duplicates
	for (long i = 0; i < list_len; i++) {
		for (long j = i + 1; j < list_len; j++) {
			if (x->list[i] == x->list[j]) {
				for (long k = j; k < list_len; k++) {
					x->list[k] = x->list[k + 1];
				}
				j--;
				list_len--;
			}
		}
	}

	//sort
	for (long i = 0; i < list_len - 1; i++) {

		//find the minimum
		min_idx = i;
		for (long j = i + 1; j < list_len; j++) {
			if (x->list[j] < x->list[min_idx]) { min_idx = j; }
		}

		//swap
		swap(&x->list[min_idx], &x->list[i]);
	}

	x->list_len = list_len;
}

void fl_dele_free(t_fl_dele *x)
{
	sysmem_freeptr(x->list);
}

void fl_dele_list_len(t_fl_dele *x, t_symbol *s, long argc, t_atom *argv)
{
	long ac = argc;
	t_atom *ap = argv;
	long new_len;

	if (ac != 1) { object_error((t_object *)x, "[1 arg] (int) list size"); return; }
	if(atom_gettype(ap)!=A_FLOAT && atom_gettype(ap)!=A_LONG) { object_warn((t_object *)x, "argument must be a number"); return; }
	
	new_len = (long)atom_getlong(ap);
	if (new_len <= 1) { object_error((t_object *)x, "number must be a positive greater than 1"); return; }

	x->list = (float *)sysmem_resizeptr(x->list, new_len * sizeof(float));
	if (!x->list) { object_error((t_object *)x, "no memory for array"); return; }

	for (long i = 0; i < new_len; i++) {
		x->list[i] = 0;
	}
	x->list_len = 0;
	x->max_len = new_len;
}

void swap(float *px, float *py)
{
	float temp = *px;
	*px = *py;
	*py = temp;
}