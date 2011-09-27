/*
 * Copyright (C) 2001-2005, Gaudenz Alder
 * Copyright (C) 2005, David Benson
 *
 * See LICENSE file in distribution for license details
 */

package borgui.graph;

import java.awt.BasicStroke;
import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Container;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;

import javax.swing.JPanel;
import javax.swing.JViewport;
import javax.swing.border.MatteBorder;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import org.jgraph.JGraph;
import org.jgraph.event.GraphLayoutCacheEvent;
import org.jgraph.event.GraphLayoutCacheListener;
import org.jgraph.event.GraphModelEvent;
import org.jgraph.event.GraphModelListener;
import org.jgraph.graph.CellView;
import org.jgraph.graph.GraphLayoutCache;

public class GraphOverviewPanelWrapper extends JPanel {
	protected GraphOverviewPanel m_overview;
	
	public GraphOverviewPanelWrapper(GraphOverviewPanel overview) {
		m_overview = overview;
	}

	public GraphOverviewPanel getOverview() {
		return m_overview;
	}

}
