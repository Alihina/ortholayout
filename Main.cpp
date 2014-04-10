// GraphDrawingProject.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <ogdf/basic/Graph.h>
#include <ogdf/basic/GraphAttributes.h>
#include <GraphIO.h>
#include <GridGraph.h>

int readCustomFormat(GridGraph &GG, Graph &G, std::string filename, int scaling = 32) {
        std::ifstream infile(filename);
        int numberVerts = 0;
        infile >> numberVerts;
        int a, b;
       
        List<node> nodes;
        char c;
        for (int i = 0; i<numberVerts; ++i) {
                infile >> a >> b;
                node n = G.newNode();   //actually make the points; mind the scaling.
                nodes.pushBack(n);
				GG.addNode(n);
                GG.x(n) = a*scaling;   
                GG.y(n) = b*scaling;                           
        }
 
       
        while (true) { // loop to read all the edges
                a=-5000;b=-5000; //super bad idea, this can be done non-shitty.
                infile >> a >> b;
                if (a==-5000 && b==-5000){
                        break;
                }
                ListIterator<node> l1 = nodes.get(a);
                ListIterator<node> l2 = nodes.get(b);
                               
				edge e = GG.addEdge(G.newEdge(*l1,*l2));

                IPolyline &p = GG.edgeline(e);
				p.pushBack(GG.getPos(*l1));
 
                infile >> std::ws; //eat WhiteSpaces
                c = infile.peek();
                if (c!='[') {//find the '['
                        return 1;
                }
                else {
                        infile.ignore(); //skip it.
                }
               
                infile >> std::ws; // we after '[' now
 
                while (infile.peek()!=']') { //are we done with this edge?
                        infile >> a >> b;
                        p.pushBack(IPoint(a*scaling,b*scaling));
                        infile >> std::ws;
                }
                infile.ignore();
                infile >> std::ws;
				p.pushBack(GG.getPos(*l2));
 
        }
        infile.close();
       
        //GA.writeGML("testout.gml");//eh, kannst du woanders hinschieben
        return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	Graph G;

	/*GridGraph GG(G);	
	readCustomFormat(GG,G,"GG.txt");
	const Graph &H = GG.constGraph();	
	std::cout << H.numberOfNodes();	
	GUI gui(800,100);
	gui.setGG(GG);
	gui.start();	
	gui.wait(100000);;*/
	return 0;
}

