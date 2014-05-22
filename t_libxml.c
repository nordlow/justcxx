/*!
 * \file t_libxml.c
 * @synopsis Navigates a tree to print element names
 * @purpose Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 */

#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlreader.h>

#include "../utils.h"
#include "../stdio_x.h"

/* ========================================================================= */

/*!
 * Print XML Attributes at \p root.
 * \param depth Depth in XML Tree
 */
void xmlAttr_print(xmlAttr * root, int depth);

/*!
 * Prints the names of the all the XML elements that are siblings or
 * children of the XML node \p a.
 *
 * \param a Initial XML Node to consider.
 * \param depth Depth in XML Tree
 */
void xmlNode_print(xmlNode * root, int depth);

/* ---------------------------- Group Separator ---------------------------- */

static int g_indent = 2;	/**< Output Indentation. */

void xmlAttr_print(xmlAttr * root, int depth)
{
  for (xmlAttr *curr = root; curr; curr = curr->next) { /* for each node */
    if (curr->type == XML_ATTRIBUTE_NODE) {
      int ns = depth * g_indent;
      spaces(ns); printf("Name: %s\n", curr->name);
      spaces(ns); printf("Value: \n");
      xmlNode_print(curr->children, depth+1);
    }
  }
}

void
xmlNode_print(xmlNode * root, int depth)
{
  for (xmlNode *curr = root; curr; curr = curr->next) { /* for each node */
    if (curr->type == XML_ELEMENT_NODE) {
      int ns = depth * g_indent;
      spaces(ns);
      printf("Name: %s  ", curr->name);
      if (curr->content) {
        printf("Content: \"%s\"  ", curr->content);
      }
      printf("Line: %d  ", curr->line);
      endline();

      xmlChar * txt = NULL;
      txt = xmlGetProp(curr, (uchar*)"name");
      spaces(ns); printf("array %s:\n", txt);
      xmlFree(txt);

      txt = xmlNodeListGetString(curr->doc, curr, 1);
      spaces(ns); printf("Text: %s\n", txt);
      xmlFree(txt);

      spaces(ns); printf("Properties:\n");
      xmlAttr_print(curr->properties, depth+1);

    }
    xmlNode_print(curr->children, depth+1);
  }
}

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Simple example to parse a file called "file.xml", 
 * walk down the DOM, and print the name of the 
 * XML elements nodes.
 */
int
main(int argc, char **argv, char *envp[])
{
  if (argc != 1 + 1) {
    printf("Usage: %s <XML-FILE>\n", argv[0]);
    return (1);
  }

  xmlDoc * doc = xmlReadFile(argv[1], NULL, 0);
  if (doc == NULL) {
    printf("error: could not parse file %s\n", argv[1]);
  }

  xmlNode * root = xmlDocGetRootElement(doc);
  xmlNode_print(root, 0);

  xmlTextReaderPtr reader = xmlReaderWalker(doc);
  xmlTextReaderClose(reader);
    
  xmlFreeDoc(doc);
  xmlCleanupParser();

  return 0;
}
