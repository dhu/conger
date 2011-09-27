/* Does nothing at all but
 * if there are not files to compile,
 * automake may fail to set $(LINK) variable
 * and then compilation fails saying:
 * rpath: command not found
 */
void do_nothing()
{

}

