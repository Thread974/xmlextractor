//
//  main.c
//  xmlextractor
//
//  Created by fredo on 10/03/2016.
//  Copyright © 2016 dalleau.re. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <libxml/xmlstring.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

struct node_info {
    xmlChar *name;
    float lat;
    float lon;
    float ele;
    int peak;
};

static struct node_info g_node_info;

void processNode(xmlTextReaderPtr reader) {
    xmlChar *name, *att1, *att2; // This is UTF8 encoded string, use BAD_CAST to cast from ANSI char *
    /* handling of a node in the tree */
    name = xmlTextReaderName(reader);
    if (!name)
        return;
    
    /*
     <node id="26863762" lat="44.1209581" lon="3.5815802" version="13" timestamp="2013-02-03T00:25:28Z" changeset="14890502" uid="16038" user="krysst">
     <tag k="ele" v="1565"/>
     <tag k="name" v="Mont Aigoual"/>
     <tag k="natural" v="peak"/>
     </node>
     */
    if (!xmlStrcmp(name, BAD_CAST "tag")) {
        att1 = xmlTextReaderGetAttribute(reader, BAD_CAST "k");
        att2 = xmlTextReaderGetAttribute(reader, BAD_CAST "v");
        /*
         printf("%d %d %s %d k:%s v:%s",
         xmlTextReaderDepth(reader),
         xmlTextReaderNodeType(reader),
         name,
         xmlTextReaderIsEmptyElement(reader),
         k ? k : BAD_CAST "NULL", v ? v : BAD_CAST "NULL");
         */
        
        if (!xmlStrcmp(att1, BAD_CAST "name")) {
            if (g_node_info.name)
                xmlFree(g_node_info.name);
            g_node_info.name = xmlStrdup(att2);
        } else if (!xmlStrcmp(att1, BAD_CAST "ele")) {
            g_node_info.ele = atof((char *)att2);
        } else if (!xmlStrcmp(att1, BAD_CAST "natural") && !xmlStrcmp(att2, BAD_CAST "peak")) {
            g_node_info.peak = 1;
        }
        if (att1)
            xmlFree(att1);
        if (att2)
            xmlFree(att2);
    } else if (!xmlStrcmp(name, BAD_CAST "node")) {
        switch (xmlTextReaderNodeType(reader)) {
            case XML_READER_TYPE_ELEMENT:
                att1 = xmlTextReaderGetAttribute(reader, BAD_CAST "lat");
                att2 = xmlTextReaderGetAttribute(reader, BAD_CAST "lon");
                if (att1 && att2) {
                    g_node_info.lat = atof((char *)att1);
                    g_node_info.lon = atof((char *)att2);
                }
                if (att1)
                    xmlFree(att1);
                if (att2)
                    xmlFree(att2);
                break;
            case XML_READER_TYPE_END_ELEMENT:
                if (g_node_info.peak) {
                    printf("{ \"%s\", %f, %f, %f },\n", g_node_info.name, g_node_info.lat, g_node_info.lon, g_node_info.ele);
                    if (g_node_info.name)
                        xmlFree(g_node_info.name);
                    memset(&g_node_info, 0, sizeof(g_node_info));
                }
                break;
        }
    }

    xmlFree(name);
}

int streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;
    
    if (filename) {
        reader = xmlNewTextReaderFilename(filename);
    } else {
        reader = xmlReaderForFd(fileno(stdin), NULL, NULL, 0);
    }
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
        arg = NULL;
    } else {
        arg = argv[1];
    }

    streamFile(arg);
    return 0;
}
