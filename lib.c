#include "ether/ether.h"

#include <glob.h>
#include <errno.h>

static eth_t
_glob(void)
{
  eth_args args = eth_start(2);

  eth_t pat = eth_arg2(args, eth_string_type);
  eth_t flag = eth_arg2(args, eth_number_type);
  
  glob_t g;
  switch (glob(eth_str_cstr(pat), eth_num_val(flag), NULL, &g))
  {
    case GLOB_NOSPACE:
      eth_throw(args, eth_system_error(ENOMEM));

    case GLOB_ABORTED:
      eth_throw(args, eth_sym("GLOB_ABORTED"));

    case GLOB_NOMATCH:
      eth_throw(args, eth_sym("GLOB_NOMATCH"));
  }

  eth_t acc = eth_nil;
  for (int i = g.gl_pathc - 1; i > 0; --i)
    acc = eth_cons(eth_str(g.gl_pathv[i]), acc);

  globfree(&g);
  eth_return(args, acc);
}

int
ether_module(eth_module *mod, eth_root *root)
{
  eth_define(mod, "__glob", eth_proc(_glob, 2));
  if (not eth_add_module_script(mod, "./lib.eth", root))
    return -1;
  return 0;
}
