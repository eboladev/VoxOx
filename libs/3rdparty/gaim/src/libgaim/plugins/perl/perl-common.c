#include "debug.h"
#include "value.h"

#include "perl-common.h"

extern PerlInterpreter *my_perl;

static GHashTable *object_stashes = NULL;

void gaim_perl_normalize_script_name(char *name)
{
	char *c;

	c = strrchr(name, '.');

	if (c != NULL)
		*c = '\0';

	for (c = name; *c != '\0'; c++) {
		if (*c != '_' && !g_ascii_isalnum(*c))
			*c = '_';
	}
}

static int
magic_free_object(pTHX_ SV *sv, MAGIC *mg)
{
	sv_setiv(sv, 0);

	return 0;
}

static MGVTBL vtbl_free_object =
{
	NULL, NULL, NULL, NULL, magic_free_object, NULL, NULL
};

static SV *
create_sv_ptr(void *object)
{
	SV *sv;

	sv = newSViv((IV)object);

	sv_magic(sv, NULL, '~', NULL, 0);

	SvMAGIC(sv)->mg_private = 0x1551; /* HF */
	SvMAGIC(sv)->mg_virtual = &vtbl_free_object;

	return sv;
}

SV *
newSVGChar(const char *str)
{
	SV *sv;

	if (str == NULL)
		return &PL_sv_undef;

	sv = newSVpv(str, 0);
	SvUTF8_on(sv);

	return sv;
}

SV *
gaim_perl_bless_object(void *object, const char *stash_name)
{
	HV *stash;
	HV *hv;

	if (object == NULL)
		return NULL;

	if (object_stashes == NULL) {
		object_stashes = g_hash_table_new(g_direct_hash, g_direct_equal);
	}

	stash = gv_stashpv(stash_name, 1);

	hv = newHV();
	hv_store(hv, "_gaim", 5, create_sv_ptr(object), 0);

	return sv_bless(newRV_noinc((SV *)hv), stash);
}

gboolean
gaim_perl_is_ref_object(SV *o)
{
	SV **sv;
	HV *hv;

	hv = hvref(o);

	if (hv != NULL) {
		sv = hv_fetch(hv, "_gaim", 5, 0);

		if (sv != NULL)
			return TRUE;
	}

	return FALSE;
}

void *
gaim_perl_ref_object(SV *o)
{
	SV **sv;
	HV *hv;
	void *p;

	if (o == NULL)
		return NULL;

	hv = hvref(o);

	if (hv == NULL)
		return NULL;

	sv = hv_fetch(hv, "_gaim", 5, 0);

	if (sv == NULL)
		croak("variable is damaged");

	p = GINT_TO_POINTER(SvIV(*sv));

	return p;
}

/*
  2003/02/06: execute_perl modified by Mark Doliner <mark@kingant.net>
		Pass parameters by pushing them onto the stack rather than
		passing an array of strings.  This way, perl scripts can
		modify the parameters and we can get the changed values
		and then shoot ourselves.  I mean, uh, use them.

  2001/06/14: execute_perl replaced by Martin Persson <mep@passagen.se>
		previous use of perl_eval leaked memory, replaced with
		a version that uses perl_call instead

  30/11/2002: execute_perl modified by Eric Timme <timothy@voidnet.com>
		args changed to char** so that we can have preparsed
  		arguments again, and many headaches ensued! This essentially
		means we replaced one hacked method with a messier hacked
		method out of perceived necessity. Formerly execute_perl
		required a single char_ptr, and it would insert it into an
		array of character pointers and NULL terminate the new array.
		Now we have to pass in pre-terminated character pointer arrays
		to accomodate functions that want to pass in multiple arguments.

		Previously arguments were preparsed because an argument list
		was constructed in the form 'arg one','arg two' and was
		executed via a call like &funcname(arglist) (see .59.x), so
		the arglist was magically pre-parsed because of the method.
		With Martin Persson's change to perl_call we now need to
		use a null terminated list of character pointers for arguments
		if we wish them to be parsed. Lacking a better way to allow
		for both single arguments and many I created a NULL terminated
		array in every function that called execute_perl and passed
		that list into the function.  In the former version a single
		character pointer was passed in, and was placed into an array
		of character pointers with two elements, with a NULL element
		tacked onto the back, but this method no longer seemed prudent.

		Enhancements in the future might be to get rid of pre-declaring
		the array sizes?  I am not comfortable enough with this
		subject to attempt it myself and hope it to stand the test
		of time.
*/
int
execute_perl(const char *function, int argc, char **args)
{
	int count = 0, i, ret_value = 1;
	SV *sv_args[argc];
	STRLEN na;
	dSP;
	PERL_SET_CONTEXT(my_perl);
	/*
	 * Set up the perl environment, push arguments onto the
	 * perl stack, then call the given function
	 */
	SPAGAIN;
	ENTER;
	SAVETMPS;
	PUSHMARK(sp);

	for (i = 0; i < argc; i++) {
		if (args[i]) {
			sv_args[i] = sv_2mortal(newSVpv(args[i], 0));
			XPUSHs(sv_args[i]);
		}
	}

	PUTBACK;
	PERL_SET_CONTEXT(my_perl);
	count = call_pv(function, G_EVAL | G_SCALAR);
	SPAGAIN;

	/*
	 * Check for "die," make sure we have 1 argument, and set our
	 * return value.
	 */
	if (SvTRUE(ERRSV)) {
		gaim_debug(GAIM_DEBUG_ERROR, "perl",
				   "Perl function %s exited abnormally: %s\n",
				   function, SvPV(ERRSV, na));
		POPs;
	} else if (count != 1) {
		/*
		 * This should NEVER happen.  G_SCALAR ensures that we WILL
		 * have 1 parameter.
		 */
		gaim_debug(GAIM_DEBUG_ERROR, "perl",
				   "Perl error from %s: expected 1 return value, "
				   "but got %d\n", function, count);
	} else
		ret_value = POPi;

	/* Check for changed arguments */
	for (i = 0; i < argc; i++) {
		if (args[i] && strcmp(args[i], SvPVX(sv_args[i]))) {
			/*
			 * Shizzel.  So the perl script changed one of the parameters,
			 * and we want this change to affect the original parameters.
			 * args[i] is just a temporary little list of pointers.  We don't
			 * want to free args[i] here because the new parameter doesn't
			 * overwrite the data that args[i] points to.  That is done by
			 * the function that called execute_perl.  I'm not explaining this
			 * very well.  See, it's aggregate...  Oh, but if 2 perl scripts
			 * both modify the data, _that's_ a memleak.  This is really kind
			 * of hackish.  I should fix it.  Look how long this comment is.
			 * Holy crap.
			 */
			args[i] = g_strdup(SvPV(sv_args[i], na));
		}
	}

	PUTBACK;
	FREETMPS;
	LEAVE;

	return ret_value;
}

#if 0
gboolean
gaim_perl_value_from_sv(GaimValue *value, SV *sv)
{
	switch (gaim_value_get_type(value))
	{
		case GAIM_TYPE_CHAR:
			if ((tmp = SvGChar(sv)) != NULL)
				gaim_value_set_char(value, tmp[0]);
			else
				return FALSE;
			break;

		case GAIM_TYPE_UCHAR:
			if ((tmp = SvPV_nolen(sv)) != NULL)
				gaim_value_set_uchar(value, tmp[0]);
			else
				return FALSE;
			break;

		case GAIM_TYPE_BOOLEAN:
			gaim_value_set_boolean(value, SvTRUE(sv));
			break;

		case GAIM_TYPE_INT:
			gaim_value_set_int(value, SvIV(sv));
			break;

		case GAIM_TYPE_UINT:
			gaim_value_set_uint(value, SvIV(sv));
			break;

		case GAIM_TYPE_LONG:
			gaim_value_set_long(value, SvIV(sv));
			break;

		case GAIM_TYPE_ULONG:
			gaim_value_set_ulong(value, SvIV(sv));
			break;

		case GAIM_TYPE_INT64:
			gaim_value_set_int64(value, SvIV(sv));
			break;

		case GAIM_TYPE_UINT64:
			gaim_value_set_uint64(value, SvIV(sv));
			break;

		case GAIM_TYPE_STRING:
			gaim_value_set_string(value, SvGChar(sv));
			break;

		case GAIM_TYPE_POINTER:
			gaim_value_set_pointer(value, (void *)SvIV(sv));
			break;

		case GAIM_TYPE_BOXED:
			if (!strcmp(gaim_value_get_specific_type(value), "SV"))
				gaim_value_set_boxed(value, (sv == &PL_sv_undef ? NULL : sv));
			else
				gaim_value_set_boxed(value, sv);
			break;

		default:
			return FALSE;
	}

	return TRUE;
}

SV *
gaim_perl_sv_from_value(const GaimValue *value, va_list list)
{
	switch (gaim_value_get_type(value))
	{
		case GAIM_TYPE_BOOLEAN:
			return newSViv(gaim_value_get_boolean(value));
			break;

		case GAIM_TYPE_INT:
			return newSViv(gaim_value_get_int(value));
			break;

		case GAIM_TYPE_UINT:
			return newSVuv(gaim_value_get_uint(value));
			break;

		case GAIM_TYPE_LONG:
			return newSViv(gaim_value_get_long(value));
			break;

		case GAIM_TYPE_ULONG:
			return newSVuv(gaim_value_get_ulong(value));
			break;

		case GAIM_TYPE_INT64:
			return newSViv(gaim_value_get_int64(value));
			break;

		case GAIM_TYPE_UINT64:
			return newSVuv(gaim_value_get_int64(value));
			break;

		case GAIM_TYPE_STRING:
			return newSVGChar(gaim_value_get_string(value));
			break;

		case GAIM_TYPE_POINTER:
			return newSViv((IV)gaim_value_get_pointer(value));
			break;

		case GAIM_TYPE_BOXED:
			if (!strcmp(gaim_value_get_specific_type(value), "SV"))
			{
				SV *sv = (SV *)gaim_perl_get_boxed(value);

				return (sv == NULL ? &PL_sv_undef : sv);
			}

			/* Uh.. I dunno. Try this? */
			return sv_2mortal(gaim_perl_bless_object(
					gaim_perl_get_boxed(value),
					gaim_value_get_specific_type(value)));

		default:
			return FALSE;
	}

	return TRUE;
}
#endif

void *
gaim_perl_data_from_sv(GaimValue *value, SV *sv)
{
	STRLEN na;

	switch (gaim_value_get_type(value)) {
		case GAIM_TYPE_BOOLEAN: return (void *)SvIV(sv);
		case GAIM_TYPE_INT:     return (void *)SvIV(sv);
		case GAIM_TYPE_UINT:    return (void *)SvUV(sv);
		case GAIM_TYPE_LONG:    return (void *)SvIV(sv);
		case GAIM_TYPE_ULONG:   return (void *)SvUV(sv);
		case GAIM_TYPE_INT64:   return (void *)SvIV(sv);
		case GAIM_TYPE_UINT64:  return (void *)SvUV(sv);
		case GAIM_TYPE_STRING:  return g_strdup((void *)SvPV(sv, na));
		case GAIM_TYPE_POINTER: return (void *)SvIV(sv);
		case GAIM_TYPE_BOXED:   return (void *)SvIV(sv);

		default:
			return NULL;
	}

	return NULL;
}

static SV *
gaim_perl_sv_from_subtype(const GaimValue *value, void *arg)
{
	const char *stash = NULL;

	switch (gaim_value_get_subtype(value)) {
		case GAIM_SUBTYPE_ACCOUNT:
			stash = "Gaim::Account";
			break;
		case GAIM_SUBTYPE_BLIST:
			stash = "Gaim::BuddyList";
			break;
		case GAIM_SUBTYPE_BLIST_BUDDY:
			stash = "Gaim::BuddyList::Buddy";
			break;
		case GAIM_SUBTYPE_BLIST_GROUP:
			stash = "Gaim::BuddyList::Group";
			break;
		case GAIM_SUBTYPE_BLIST_CHAT:
			stash = "Gaim::BuddyList::Chat";
			break;
		case GAIM_SUBTYPE_BUDDY_ICON:
			stash = "Gaim::Buddy::Icon";
			break;
		case GAIM_SUBTYPE_CONNECTION:
			stash = "Gaim::Connection";
			break;
		case GAIM_SUBTYPE_CONVERSATION:
			stash = "Gaim::Conversation";
			break;
		case GAIM_SUBTYPE_PLUGIN:
			stash = "Gaim::Plugin";
			break;
		case GAIM_SUBTYPE_BLIST_NODE:
			stash = "Gaim::BuddyList::Node";
			break;
		case GAIM_SUBTYPE_CIPHER:
			stash = "Gaim::Cipher";
			break;
		case GAIM_SUBTYPE_STATUS:
			stash = "Gaim::Status";
			break;
		case GAIM_SUBTYPE_LOG:
			stash = "Gaim::Log";
			break;
		case GAIM_SUBTYPE_XFER:
			stash = "Gaim::Xfer";
			break;
		case GAIM_SUBTYPE_XMLNODE:
			stash = "Gaim::XMLNode";
			break;

		default:
			stash = "Gaim"; /* ? */
	}

	return sv_2mortal(gaim_perl_bless_object(arg, stash));
}

SV *
gaim_perl_sv_from_vargs(const GaimValue *value, va_list *args, void ***copy_arg)
{
	if (gaim_value_is_outgoing(value)) {
		switch (gaim_value_get_type(value)) {
			case GAIM_TYPE_SUBTYPE:
				if ((*copy_arg = va_arg(*args, void **)) == NULL)
					return &PL_sv_undef;

				return gaim_perl_sv_from_subtype(value, *(void **)*copy_arg);

			case GAIM_TYPE_BOOLEAN:
				if ((*copy_arg = (void *)va_arg(*args, gboolean *)) == NULL)
					return &PL_sv_undef;

				return newSViv(*(gboolean *)*copy_arg);

			case GAIM_TYPE_INT:
				if ((*copy_arg = (void *)va_arg(*args, int *)) == NULL)
					return &PL_sv_undef;

				return newSViv(*(int *)*copy_arg);

			case GAIM_TYPE_UINT:
				if ((*copy_arg = (void *)va_arg(*args, unsigned int *)) == NULL)
					return &PL_sv_undef;

				return newSVuv(*(unsigned int *)*copy_arg);

			case GAIM_TYPE_LONG:
				if ((*copy_arg = (void *)va_arg(*args, long *)) == NULL)
					return &PL_sv_undef;

				return newSViv(*(long *)*copy_arg);

			case GAIM_TYPE_ULONG:
				if ((*copy_arg = (void *)va_arg(*args,
												unsigned long *)) == NULL)
					return &PL_sv_undef;

				return newSVuv(*(unsigned long *)*copy_arg);

			case GAIM_TYPE_INT64:
				if ((*copy_arg = (void *)va_arg(*args, gint64 *)) == NULL)
					return &PL_sv_undef;

				return newSViv(*(gint64 *)*copy_arg);

			case GAIM_TYPE_UINT64:
				if ((*copy_arg = (void *)va_arg(*args, guint64 *)) == NULL)
					return &PL_sv_undef;

				return newSVuv(*(guint64 *)*copy_arg);

			case GAIM_TYPE_STRING:
				if ((*copy_arg = (void *)va_arg(*args, char **)) == NULL)
					return &PL_sv_undef;

				return newSVGChar(*(char **)*copy_arg);

			case GAIM_TYPE_POINTER:
				if ((*copy_arg = va_arg(*args, void **)) == NULL)
					return &PL_sv_undef;

				return newSViv((IV)*(void **)*copy_arg);

			case GAIM_TYPE_BOXED:
				/* Uh.. I dunno. Try this? */
				if ((*copy_arg = va_arg(*args, void **)) == NULL)
					return &PL_sv_undef;

				return sv_2mortal(gaim_perl_bless_object(
						*(void **)*copy_arg,
						gaim_value_get_specific_type(value)));

			default:
				/* If this happens, things are going to get screwed up... */
				return NULL;
		}
	} else {
		switch (gaim_value_get_type(value)) {
			case GAIM_TYPE_SUBTYPE:
				if ((*copy_arg = va_arg(*args, void *)) == NULL)
					return &PL_sv_undef;

				return gaim_perl_sv_from_subtype(value, *copy_arg);

			case GAIM_TYPE_BOOLEAN:
				*copy_arg = GINT_TO_POINTER( va_arg(*args, gboolean) );

				return newSViv((gboolean)GPOINTER_TO_INT(*copy_arg));

			case GAIM_TYPE_INT:
				*copy_arg = GINT_TO_POINTER( va_arg(*args, int) );

				return newSViv(GPOINTER_TO_INT(*copy_arg));

			case GAIM_TYPE_UINT:
				*copy_arg = GUINT_TO_POINTER(va_arg(*args, unsigned int));

				return newSVuv(GPOINTER_TO_UINT(*copy_arg));

			case GAIM_TYPE_LONG:
				*copy_arg = (void *)va_arg(*args, long);

				return newSViv((long)*copy_arg);

			case GAIM_TYPE_ULONG:
				*copy_arg = (void *)va_arg(*args, unsigned long);

				return newSVuv((unsigned long)*copy_arg);

			case GAIM_TYPE_INT64:
#if 0
				/* XXX This yells and complains. */
				*copy_arg = va_arg(*args, gint64);

				return newSViv(*copy_arg);
#endif
				break;

			case GAIM_TYPE_UINT64:
				/* XXX This also yells and complains. */
#if 0
				*copy_arg = (void *)va_arg(*args, guint64);

				return newSVuv(*copy_arg);
#endif
				break;

			case GAIM_TYPE_STRING:
				if ((*copy_arg = (void *)va_arg(*args, char *)) == NULL)
					return &PL_sv_undef;

				return newSVGChar((char *)*copy_arg);

			case GAIM_TYPE_POINTER:
				if ((*copy_arg = (void *)va_arg(*args, void *)) == NULL)
					return &PL_sv_undef;

				return newSViv((IV)*copy_arg);

			case GAIM_TYPE_BOXED:
				/* Uh.. I dunno. Try this? */
				if ((*copy_arg = (void *)va_arg(*args, void *)) == NULL)
					return &PL_sv_undef;

				return sv_2mortal(gaim_perl_bless_object(*copy_arg,
						gaim_value_get_specific_type(value)));

			default:
				/* If this happens, things are going to get screwed up... */
				return NULL;
		}
	}

	return NULL;
}
