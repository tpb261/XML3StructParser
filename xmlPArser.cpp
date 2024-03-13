#include <tinyxml2.h>
#include <stdio.h>
#include<vector>

using namespace std;
using namespace tinyxml2;
typedef struct _xmlParserStruct_
{
    char *tag;
    char *attributes; /* space delimited attributes to read */
    int   num;
    int   size;
    int   offset;
    struct _xmlParserStruct_ *subNode;
    int (*readElement) ( XMLParser *Node, XMLElement *root, char *base);
} XMLParser;

int readInt(XMLParser *Node, XMLElement *root, char *base)
{
    const char *s = root->GetText();
    int tmp = s?atoi(s):0;    
    memcpy(base, &tmp, sizeof(tmp));
}

int readFloat(XMLParser *Node, XMLElement *root, char *base)
{
    const char *s = root->GetText();
    float tmp = s?atof(s):0;    
    memcpy(base, &tmp, sizeof(tmp));
}

int readChar(XMLParser *Node, XMLElement *root, char *base)
{
    const char *s = root->GetText();
    char tmp = s?s[0]:0;    
    *base = tmp;
}

int readString(XMLParser *Node, XMLElement *root, char *base)
{
    const char *s = root->GetText();
    int len = strlen(s);
    char *tmp = (char*)calloc(strlen(s),1);
    memcpy(base, tmp, sizeof(tmp));
}

int readArray(XMLParser *Node, XMLElement *root, char *base)
{
    int len = Node[0].num;
    XMLElement *p = NULL;
    if (!len)
     {
        for(p=root->FirstChildElement(Node[0].tag); p; p=p->NextSiblingElement(), ++len)
        {
            Node[0].readElement(Node[0].subNode, p, base+Node[0].size*len);    
        }
     }
    else
    {
        for(int i = 0; i<len; ++i, p=p->NextSiblingElement())
        {
            int index = p->IntAttribute(Node[0].attributes);
            Node[0].readElement(Node[0].subNode, p, base+Node[0].size*index);
        }
    }
}

int parseXMLTreeToStruct(
    XMLParser  *Node,
    XMLElement *root,
    char       *base
    )
{
    for(int i =0; Node[i].tag; i++)
    {
        XMLParser leaf = Node[i];
        XMLElement *p = root->FirstChildElement(Node[i].tag);
        char *start = base+Node[i].offset;
        if(!p) continue;
        if ( Node[i].num == 1 && Node[i].subNode != NULL)
        {
            /* parse sub tree in XML */
            parseXMLTreeToStruct(Node[i].subNode, p, start);
        }
        else if (Node[i].num != 1 && Node[i].subNode != NULL)
        {
            /* read an array of Structs */
            readArray(Node+i, p, start);

        }
        else if ( Node[i].num == 1 && Node[i].subNode == NULL)
        {
            /* read a single element */
            Node[i].readElement(Node+i, p, start);
        }
    }
}

int main()
{

}