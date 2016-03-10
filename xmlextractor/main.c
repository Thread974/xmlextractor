//
//  main.c
//  xmlextractor
//
//  Created by fredo on 10/03/2016.
//  Copyright © 2016 dalleau.re. All rights reserved.
//

#include <stdio.h>
#include <libxml/xmlstring.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

void processNode(xmlTextReaderPtr reader) {
    xmlChar *name, *value; // This is UTF8 encoded string, use BAD_CAST to cast from ANSI char *
    /* handling of a node in the tree */
    name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
    value = xmlTextReaderValue(reader);
    
    /*
     <node id="26863762" lat="44.1209581" lon="3.5815802" version="13" timestamp="2013-02-03T00:25:28Z" changeset="14890502" uid="16038" user="krysst">
     <tag k="ele" v="1565"/>
     <tag k="name" v="Mont Aigoual"/>
     <tag k="natural" v="peak"/>
     </node>
     */
    if (!xmlStrcmp(name, BAD_CAST "tag")) {
        printf("%d %d %s %s %d",
               xmlTextReaderDepth(reader),
               xmlTextReaderNodeType(reader),
               xmlTextReaderConstPrefix(reader),
               name,
               xmlTextReaderIsEmptyElement(reader));
        if (value == NULL)
            printf("\n");
        else {
            printf(" %s\n", value);
            xmlFree(value);
        }
    }
    xmlFree(name);
}

int streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;
    
    reader = xmlNewTextReaderFilename(filename);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    } else {
        printf("Unable to open %s\n", filename);
    }

    return ret;
}

int main(int argc, const char * argv[]) {
    const char *arg;

    // insert code here...
    if (argc < 2) {
        arg = "geo.xml";
    } else {
        arg = argv[1];
    }

    streamFile(arg);
    return 0;
}
