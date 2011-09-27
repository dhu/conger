package borgui.graph;

import javax.swing.*;
import java.awt.geom.Rectangle2D;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import org.jgraph.JGraph;
import org.jgraph.graph.*;
import org.jgraph.cellview.*;
import borgui.xml.*;
import borgui.common.*;

public class BorealisCellViewFactory extends DefaultCellViewFactory {



	public BorealisCellViewFactory() {
		
	}

        protected VertexView createVertexView(Object v) {

		if (v instanceof BoxGraphCell) {
			return new JGraphRoundRectView(v);
		} else if (v instanceof InputGraphCell) {
			return new JGraphDiamondView(v);
		} else if (v instanceof OutputGraphCell) {
			return new JGraphDiamondView(v);
		} else if (v instanceof TableGraphCell) {
			return new JGraphEllipseView(v);
		}

                return super.createVertexView(v);
         }

	 protected PortView createPortView(Object p) {

	/*	
		if (p instanceof InputPort) {
			return new VertexView(p);
		} else if (p instanceof OutputPort) {
			return new VertexView(p);
		}*/

		return super.createPortView(p);

	 }



  }	



