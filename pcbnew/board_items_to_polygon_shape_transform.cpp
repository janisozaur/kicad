/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2009-2018 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

/***
 * @file board_items_to_polygon_shape_transform.cpp
 * @brief function to convert shapes of items ( pads, tracks... ) to polygons
 */

/* Function to convert pad and track shapes to polygons
 * Used to fill zones areas and in 3D viewer
 */
#include <vector>

#include <fctsys.h>
#include <bezier_curves.h>
#include <base_units.h>     // for IU_PER_MM
#include <draw_graphic_text.h>
#include <pcbnew.h>
#include <pcb_edit_frame.h>
#include <trigo.h>
#include <class_board.h>
#include <class_pad.h>
#include <class_track.h>
#include <class_drawsegment.h>
#include <class_pcb_text.h>
#include <class_zone.h>
#include <class_module.h>
#include <class_edge_mod.h>
#include <convert_basic_shapes_to_polygon.h>
#include <geometry/geometry_utils.h>

// A helper struct for the callback function
// These variables are parameters used in addTextSegmToPoly.
// But addTextSegmToPoly is a call-back function,
// so we cannot send them as arguments.
struct TSEGM_2_POLY_PRMS {
    int m_textWidth;
    int m_textCircle2SegmentCount;
    SHAPE_POLY_SET* m_cornerBuffer;
};
TSEGM_2_POLY_PRMS prms;

// This is a call back function, used by DrawGraphicText to draw the 3D text shape:
static void addTextSegmToPoly( int x0, int y0, int xf, int yf, void* aData )
{
    TSEGM_2_POLY_PRMS* prm = static_cast<TSEGM_2_POLY_PRMS*>( aData );
    TransformRoundedEndsSegmentToPolygon( *prm->m_cornerBuffer,
                                           wxPoint( x0, y0), wxPoint( xf, yf ),
                                           prm->m_textCircle2SegmentCount, prm->m_textWidth );
}


void BOARD::ConvertBrdLayerToPolygonalContours( PCB_LAYER_ID aLayer, SHAPE_POLY_SET& aOutlines )
{
    // convert tracks and vias:
    for( TRACK* track = m_Track; track != NULL; track = track->Next() )
    {
        if( !track->IsOnLayer( aLayer ) )
            continue;

        track->TransformShapeWithClearanceToPolygon( aOutlines, 0 );
    }

    // convert pads
    for( MODULE* module = m_Modules; module != NULL; module = module->Next() )
    {
        module->TransformPadsShapesWithClearanceToPolygon( aLayer, aOutlines, 0 );

        // Micro-wave modules may have items on copper layers
        module->TransformGraphicShapesWithClearanceToPolygonSet( aLayer, aOutlines, 0 );
    }

    // convert copper zones
    for( int ii = 0; ii < GetAreaCount(); ii++ )
    {
        ZONE_CONTAINER* zone = GetArea( ii );
        PCB_LAYER_ID        zonelayer = zone->GetLayer();

        if( zonelayer == aLayer )
            zone->TransformSolidAreasShapesToPolygonSet( aOutlines );
    }

    // convert graphic items on copper layers (texts)
    for( BOARD_ITEM* item = m_Drawings; item; item = item->Next() )
    {
        if( !item->IsOnLayer( aLayer ) )
            continue;

        switch( item->Type() )
        {
        case PCB_LINE_T:
            ( (DRAWSEGMENT*) item )->TransformShapeWithClearanceToPolygon( aOutlines, 0 );
            break;

        case PCB_TEXT_T:
            ( (TEXTE_PCB*) item )->TransformShapeWithClearanceToPolygonSet( aOutlines, 0 );
            break;

        default:
            break;
        }
    }
}


void MODULE::TransformPadsShapesWithClearanceToPolygon( PCB_LAYER_ID aLayer,
        SHAPE_POLY_SET& aCornerBuffer, int aInflateValue, int aMaxError,
        bool aSkipNPTHPadsWihNoCopper ) const
{
    D_PAD* pad = PadsList();

    wxSize margin;
    for( ; pad != NULL; pad = pad->Next() )
    {
        if( aLayer != UNDEFINED_LAYER && !pad->IsOnLayer(aLayer) )
            continue;

        // NPTH pads are not drawn on layers if the shape size and pos is the same
        // as their hole:
        if( aSkipNPTHPadsWihNoCopper && pad->GetAttribute() == PAD_ATTRIB_HOLE_NOT_PLATED )
        {
            if( pad->GetDrillSize() == pad->GetSize() && pad->GetOffset() == wxPoint( 0, 0 ) )
            {
                switch( pad->GetShape() )
                {
                case PAD_SHAPE_CIRCLE:
                    if( pad->GetDrillShape() == PAD_DRILL_SHAPE_CIRCLE )
                        continue;
                    break;

                case PAD_SHAPE_OVAL:
                    if( pad->GetDrillShape() != PAD_DRILL_SHAPE_CIRCLE )
                        continue;
                    break;

                default:
                    break;
                }
            }
        }

        switch( aLayer )
        {
        case F_Mask:
        case B_Mask:
            margin.x = margin.y = pad->GetSolderMaskMargin() + aInflateValue;
            break;

        case F_Paste:
        case B_Paste:
            margin = pad->GetSolderPasteMargin();
            margin.x += aInflateValue;
            margin.y += aInflateValue;
            break;

        default:
            margin.x = margin.y = aInflateValue;
            break;
        }

        pad->BuildPadShapePolygon( aCornerBuffer, margin );
    }
}

/* generate shapes of graphic items (outlines) on layer aLayer as polygons,
 * and adds these polygons to aCornerBuffer
 * aCornerBuffer = the buffer to store polygons
 * aInflateValue = a value to inflate shapes
 * aCircleToSegmentsCount = number of segments to approximate a circle
 * aCorrectionFactor = the correction to apply to the circle radius
 *  to generate the polygon.
 *  if aCorrectionFactor = 1.0, the polygon is inside the circle
 *  the radius of circle approximated by segments is
 *  initial radius * aCorrectionFactor
 */
void MODULE::TransformGraphicShapesWithClearanceToPolygonSet( PCB_LAYER_ID aLayer,
        SHAPE_POLY_SET& aCornerBuffer, int aInflateValue, int aError, bool aIncludeText ) const
{
    std::vector<TEXTE_MODULE *> texts;  // List of TEXTE_MODULE to convert
    EDGE_MODULE* outline;

    for( EDA_ITEM* item = GraphicalItemsList(); item != NULL; item = item->Next() )
    {
        switch( item->Type() )
        {
        case PCB_MODULE_TEXT_T:
        {
            TEXTE_MODULE* text = static_cast<TEXTE_MODULE*>( item );

            if( ( aLayer != UNDEFINED_LAYER && text->GetLayer() == aLayer ) && text->IsVisible() )
                texts.push_back( text );

            break;
        }

        case PCB_MODULE_EDGE_T:
            outline = (EDGE_MODULE*) item;

            if( aLayer != UNDEFINED_LAYER && outline->GetLayer() != aLayer )
                break;

            outline->TransformShapeWithClearanceToPolygon( aCornerBuffer, 0, aError );
            break;

        default:
            break;
        }
    }

    if( !aIncludeText )
        return;

    // Convert texts sur modules
    if( Reference().GetLayer() == aLayer && Reference().IsVisible() )
        texts.push_back( &Reference() );

    if( Value().GetLayer() == aLayer && Value().IsVisible() )
        texts.push_back( &Value() );

    prms.m_cornerBuffer = &aCornerBuffer;

    for( unsigned ii = 0; ii < texts.size(); ii++ )
    {
        TEXTE_MODULE *textmod = texts[ii];
        prms.m_textWidth  = textmod->GetThickness() + ( 2 * aInflateValue );
        prms.m_textCircle2SegmentCount =
                std::max( GetArcToSegmentCount( prms.m_textWidth / 2, aError, 360.0 ), 6 );
        wxSize size = textmod->GetTextSize();

        if( textmod->IsMirrored() )
            size.x = -size.x;

        DrawGraphicText( NULL, NULL, textmod->GetTextPos(), BLACK,
                         textmod->GetShownText(), textmod->GetDrawRotation(), size,
                         textmod->GetHorizJustify(), textmod->GetVertJustify(),
                         textmod->GetThickness(), textmod->IsItalic(),
                         true, addTextSegmToPoly, &prms );
    }

}


// Same as function TransformGraphicShapesWithClearanceToPolygonSet but
// this only render text
void MODULE::TransformGraphicTextWithClearanceToPolygonSet(
        PCB_LAYER_ID aLayer, SHAPE_POLY_SET& aCornerBuffer, int aInflateValue, int aError ) const
{
    std::vector<TEXTE_MODULE *> texts;  // List of TEXTE_MODULE to convert

    for( EDA_ITEM* item = GraphicalItemsList(); item != NULL; item = item->Next() )
    {
        switch( item->Type() )
        {
        case PCB_MODULE_TEXT_T:
            {
                TEXTE_MODULE* text = static_cast<TEXTE_MODULE*>( item );

                if( text->GetLayer() == aLayer && text->IsVisible() )
                    texts.push_back( text );

                break;
            }

        case PCB_MODULE_EDGE_T:
                // This function does not render this
                break;

            default:
                break;
        }
    }

    // Convert texts sur modules
    if( Reference().GetLayer() == aLayer && Reference().IsVisible() )
        texts.push_back( &Reference() );

    if( Value().GetLayer() == aLayer && Value().IsVisible() )
        texts.push_back( &Value() );

    prms.m_cornerBuffer = &aCornerBuffer;

    for( unsigned ii = 0; ii < texts.size(); ii++ )
    {
        TEXTE_MODULE *textmod = texts[ii];
        prms.m_textWidth = textmod->GetThickness() + ( 2 * aInflateValue );
        prms.m_textCircle2SegmentCount =
                std::max( GetArcToSegmentCount( prms.m_textWidth / 2, aError, 360.0 ), 6 );
        wxSize size = textmod->GetTextSize();

        if( textmod->IsMirrored() )
            size.x = -size.x;

        DrawGraphicText( NULL, NULL, textmod->GetTextPos(), BLACK,
                         textmod->GetShownText(), textmod->GetDrawRotation(), size,
                         textmod->GetHorizJustify(), textmod->GetVertJustify(),
                         textmod->GetThickness(), textmod->IsItalic(),
                         true, addTextSegmToPoly, &prms );
    }

}


void ZONE_CONTAINER::TransformSolidAreasShapesToPolygonSet(
        SHAPE_POLY_SET& aCornerBuffer, int aError ) const
{
    if( GetFilledPolysList().IsEmpty() )
        return;

    int numSegs = std::max( GetArcToSegmentCount( GetMinThickness() / 2, aError, 360.0 ), 6 );

    // add filled areas polygons
    aCornerBuffer.Append( m_FilledPolysList );

    // add filled areas outlines, which are drawn with thick lines
    for( int i = 0; i < m_FilledPolysList.OutlineCount(); i++ )
    {
        const SHAPE_LINE_CHAIN& path = m_FilledPolysList.COutline( i );

        for( int j = 0; j < path.PointCount(); j++ )
        {
            const VECTOR2I& a = path.CPoint( j );
            const VECTOR2I& b = path.CPoint( j + 1 );

            TransformRoundedEndsSegmentToPolygon( aCornerBuffer, wxPoint( a.x, a.y ),
                    wxPoint( b.x, b.y ), numSegs, GetMinThickness() );
        }
    }
}


void EDA_TEXT::TransformBoundingBoxWithClearanceToPolygon(
        SHAPE_POLY_SET* aCornerBuffer, int aClearanceValue ) const
{
    // Oh dear.  When in UTF-8 mode, wxString puts string iterators in a linked list, and
    // that linked list is not thread-safe.
    std::lock_guard<std::mutex> guard( m_mutex );

    if( GetText().Length() == 0 )
        return;

    wxPoint  corners[4];    // Buffer of polygon corners

    EDA_RECT rect = GetTextBox( -1 );
    rect.Inflate( aClearanceValue );
    corners[0].x = rect.GetOrigin().x;
    corners[0].y = rect.GetOrigin().y;
    corners[1].y = corners[0].y;
    corners[1].x = rect.GetRight();
    corners[2].x = corners[1].x;
    corners[2].y = rect.GetBottom();
    corners[3].y = corners[2].y;
    corners[3].x = corners[0].x;

    aCornerBuffer->NewOutline();

    for( int ii = 0; ii < 4; ii++ )
    {
        // Rotate polygon
        RotatePoint( &corners[ii].x, &corners[ii].y, GetTextPos().x, GetTextPos().y, GetTextAngle() );
        aCornerBuffer->Append( corners[ii].x, corners[ii].y );
    }
}


/* Function TransformShapeWithClearanceToPolygonSet
 * Convert the text shape to a set of polygons (one by segment)
 * Used in filling zones calculations and 3D view
 * Circles and arcs are approximated by segments
 * aCornerBuffer = SHAPE_POLY_SET to store the polygon corners
 * aClearanceValue = the clearance around the text
 * aCircleToSegmentsCount = the number of segments to approximate a circle
 * aCorrectionFactor = the correction to apply to circles radius to keep
 * clearance when the circle is approximated by segment bigger or equal
 * to the real clearance value (usually near from 1.0)
 */

void TEXTE_PCB::TransformShapeWithClearanceToPolygonSet(
        SHAPE_POLY_SET& aCornerBuffer, int aClearanceValue, int aError ) const
{
    wxSize size = GetTextSize();

    if( IsMirrored() )
        size.x = -size.x;

    prms.m_cornerBuffer = &aCornerBuffer;
    prms.m_textWidth = GetThickness() + ( 2 * aClearanceValue );
    prms.m_textCircle2SegmentCount =
            std::max( GetArcToSegmentCount( prms.m_textWidth / 2, aError, 360.0 ), 6 );
    COLOR4D color = COLOR4D::BLACK;  // not actually used, but needed by DrawGraphicText

    if( IsMultilineAllowed() )
    {
        wxArrayString strings_list;
        wxStringSplit( GetShownText(), strings_list, '\n' );
        std::vector<wxPoint> positions;
        positions.reserve( strings_list.Count() );
        GetPositionsOfLinesOfMultilineText( positions, strings_list.Count() );

        for( unsigned ii = 0; ii < strings_list.Count(); ii++ )
        {
            wxString txt = strings_list.Item( ii );
            DrawGraphicText( NULL, NULL, positions[ii], color,
                             txt, GetTextAngle(), size,
                             GetHorizJustify(), GetVertJustify(),
                             GetThickness(), IsItalic(),
                             true, addTextSegmToPoly, &prms );
        }
    }
    else
    {
        DrawGraphicText( NULL, NULL, GetTextPos(), color,
                         GetShownText(), GetTextAngle(), size,
                         GetHorizJustify(), GetVertJustify(),
                         GetThickness(), IsItalic(),
                         true, addTextSegmToPoly, &prms );
    }
}


void DRAWSEGMENT::TransformShapeWithClearanceToPolygon(
        SHAPE_POLY_SET& aCornerBuffer, int aClearanceValue, int aError, bool ignoreLineWidth ) const
{
    // The full width of the lines to create:
    int linewidth = ignoreLineWidth ? 0 : m_Width;

    linewidth += 2 * aClearanceValue;

    int    numSegs = std::max( GetArcToSegmentCount( linewidth / 2, aError, 360.0 ), 6 );
    double correction = GetCircletoPolyCorrectionFactor( numSegs );

    // Creating a reliable clearance shape for circles and arcs is not so easy, due to
    // the error created by segment approximation.
    // for a circle this is not so hard: create a polygon from a circle slightly bigger:
    // thickness = linewidth + s_error_max, and radius = initial radius + s_error_max/2
    // giving a shape with a suitable internal radius and external radius
    // For an arc this is more tricky: TODO

    switch( m_Shape )
    {
    case S_CIRCLE:
        TransformRingToPolygon(
                aCornerBuffer, GetCenter(), GetRadius(), numSegs, correction * linewidth );
        break;

    case S_ARC:
        TransformArcToPolygon(
                aCornerBuffer, GetCenter(), GetArcStart(), m_Angle, numSegs, linewidth );
        break;

    case S_SEGMENT:
        TransformOvalClearanceToPolygon(
                aCornerBuffer, m_Start, m_End, linewidth, numSegs, correction );
        break;

    case S_POLYGON:
        if( IsPolyShapeValid() )
        {
            // The polygon is expected to be a simple polygon
            // not self intersecting, no hole.
            MODULE* module = GetParentModule();     // NULL for items not in footprints
            double orientation = module ? module->GetOrientation() : 0.0;
            wxPoint offset;

            if( module )
                offset = module->GetPosition();

            // Build the polygon with the actual position and orientation:
            std::vector< wxPoint> poly;
            poly = BuildPolyPointsList();

            for( unsigned ii = 0; ii < poly.size(); ii++ )
            {
                RotatePoint( &poly[ii], orientation );
                poly[ii] += offset;
            }

            // If the polygon is not filled, treat it as a closed set of lines
            if( !IsPolygonFilled() )
            {
                for( size_t ii = 1; ii < poly.size(); ii++ )
                {
                    TransformOvalClearanceToPolygon(
                            aCornerBuffer, poly[ii - 1], poly[ii], linewidth, numSegs, correction );
                }

                TransformOvalClearanceToPolygon(
                        aCornerBuffer, poly.back(), poly.front(), linewidth, numSegs, correction );
                break;
            }

            // Generate polygons for the outline + clearance
            // This code is compatible with a polygon with holes linked to external outline
            // by overlapping segments.

            // Insert the initial polygon:
            aCornerBuffer.NewOutline();

            for( unsigned ii = 0; ii < poly.size(); ii++ )
                aCornerBuffer.Append( poly[ii].x, poly[ii].y );

            if( linewidth )     // Add thick outlines
            {
                wxPoint corner1( poly[poly.size()-1] );

                for( unsigned ii = 0; ii < poly.size(); ii++ )
                {
                    wxPoint corner2( poly[ii] );

                    if( corner2 != corner1 )
                    {
                        TransformRoundedEndsSegmentToPolygon(
                                aCornerBuffer, corner1, corner2, numSegs, linewidth );
                    }

                    corner1 = corner2;
                }
            }
        }
        break;

    case S_CURVE:       // Bezier curve
        {
            std::vector<wxPoint> ctrlPoints = { m_Start, m_BezierC1, m_BezierC2, m_End };
            BEZIER_POLY converter( ctrlPoints );
            std::vector< wxPoint> poly;
            converter.GetPoly( poly, m_Width );

            for( unsigned ii = 1; ii < poly.size(); ii++ )
            {
                TransformRoundedEndsSegmentToPolygon(
                        aCornerBuffer, poly[ii - 1], poly[ii], numSegs, linewidth );
            }
        }
        break;

    default:
        break;
    }
}


void TRACK::TransformShapeWithClearanceToPolygon(
        SHAPE_POLY_SET& aCornerBuffer, int aClearanceValue, int aError, bool ignoreLineWidth ) const
{
    wxASSERT_MSG( !ignoreLineWidth, "IgnoreLineWidth has no meaning for tracks." );

    int    radius = ( m_Width / 2 ) + aClearanceValue;
    int    numSegs = std::max( GetArcToSegmentCount( radius, aError, 360.0 ), 6 );
    double correction = GetCircletoPolyCorrectionFactor( numSegs );

    switch( Type() )
    {
    case PCB_VIA_T:
    {
        radius = KiROUND( radius * correction );
        TransformCircleToPolygon( aCornerBuffer, m_Start, radius, numSegs );
    }
        break;

    default:
        TransformOvalClearanceToPolygon( aCornerBuffer, m_Start, m_End,
                m_Width + ( 2 * aClearanceValue ), numSegs, correction );
        break;
    }
}


void D_PAD::TransformShapeWithClearanceToPolygon(
        SHAPE_POLY_SET& aCornerBuffer, int aClearanceValue, int aError, bool ignoreLineWidth ) const
{
    wxASSERT_MSG( !ignoreLineWidth, "IgnoreLineWidth has no meaning for pads." );

    double  angle = m_Orient;
    int     dx = (m_Size.x / 2) + aClearanceValue;
    int     dy = (m_Size.y / 2) + aClearanceValue;

    wxPoint padShapePos = ShapePos();               /* Note: for pad having a shape offset,
                                                     * the pad position is NOT the shape position */

    switch( GetShape() )
    {
    case PAD_SHAPE_CIRCLE:
    {
        int    numSegs = std::max( GetArcToSegmentCount( dx, aError, 360.0 ), 6 );
        double correction = GetCircletoPolyCorrectionFactor( numSegs );
        dx = KiROUND( dx * correction );
        TransformCircleToPolygon( aCornerBuffer, padShapePos, dx, numSegs );
    }
        break;

    case PAD_SHAPE_OVAL:
        // An oval pad has the same shape as a segment with rounded ends
        {
        int width;
        wxPoint shape_offset;
        if( dy > dx )   // Oval pad X/Y ratio for choosing translation axis
        {
            shape_offset.y = dy - dx;
            width = dx * 2;
        }
        else    //if( dy <= dx )
        {
            shape_offset.x = dy - dx;
            width = dy * 2;
        }

        int    numSegs = std::max( GetArcToSegmentCount( width / 2, aError, 360.0 ), 6 );
        double correction = GetCircletoPolyCorrectionFactor( numSegs );

        RotatePoint( &shape_offset, angle );
        wxPoint start = padShapePos - shape_offset;
        wxPoint end = padShapePos + shape_offset;
        TransformOvalClearanceToPolygon( aCornerBuffer, start, end, width, numSegs, correction );
        }
        break;

    case PAD_SHAPE_TRAPEZOID:
    case PAD_SHAPE_RECT:
    {
        wxPoint corners[4];
        BuildPadPolygon( corners, wxSize( 0, 0 ), angle );

        SHAPE_POLY_SET outline;
        outline.NewOutline();

        for( int ii = 0; ii < 4; ii++ )
        {
            corners[ii] += padShapePos;
            outline.Append( corners[ii].x, corners[ii].y );
        }

        int    numSegs = std::max( GetArcToSegmentCount( aClearanceValue, aError, 360.0 ), 6 );
        double correction = GetCircletoPolyCorrectionFactor( numSegs );

        int rounding_radius = KiROUND( aClearanceValue * correction );
        outline.Inflate( rounding_radius, numSegs );

        aCornerBuffer.Append( outline );
    }
        break;

    case PAD_SHAPE_CHAMFERED_RECT:
    case PAD_SHAPE_ROUNDRECT:
    {
        SHAPE_POLY_SET outline;
        int            radius = GetRoundRectCornerRadius() + aClearanceValue;
        int            numSegs = std::max( GetArcToSegmentCount( radius, aError, 360.0 ), 6 );
        double         correction = GetCircletoPolyCorrectionFactor( numSegs );
        int            clearance = KiROUND( aClearanceValue * correction );
        int            rounding_radius = GetRoundRectCornerRadius() + clearance;
        wxSize         shapesize( m_Size );

        shapesize.x += clearance*2;
        shapesize.y += clearance*2;
        bool doChamfer = GetShape() == PAD_SHAPE_CHAMFERED_RECT;

        TransformRoundChamferedRectToPolygon( outline, padShapePos, shapesize, angle,
                rounding_radius, doChamfer ? GetChamferRectRatio() : 0.0,
                doChamfer ? GetChamferPositions() : 0, numSegs );

        aCornerBuffer.Append( outline );
    }
        break;

    case PAD_SHAPE_CUSTOM:
    {
        int    numSegs = std::max( GetArcToSegmentCount( aClearanceValue, aError, 360.0 ), 6 );
        double correction = GetCircletoPolyCorrectionFactor( numSegs );
        int    clearance = KiROUND( aClearanceValue * correction );
        SHAPE_POLY_SET outline;     // Will contain the corners in board coordinates
        outline.Append( m_customShapeAsPolygon );
        CustomShapeAsPolygonToBoardPosition( &outline, GetPosition(), GetOrientation() );
        outline.Simplify( SHAPE_POLY_SET::PM_FAST );
        outline.Inflate( clearance, numSegs );
        outline.Fracture( SHAPE_POLY_SET::PM_FAST );
        aCornerBuffer.Append( outline );
    }
        break;
    }
}



/*
 * Function BuildPadShapePolygon
 * Build the Corner list of the polygonal shape,
 * depending on shape, extra size (clearance ...) pad and orientation
 * Note: for Round and oval pads this function is equivalent to
 * TransformShapeWithClearanceToPolygon, but not for other shapes
 */
void D_PAD::BuildPadShapePolygon(
        SHAPE_POLY_SET& aCornerBuffer, wxSize aInflateValue, int aError ) const
{
    wxPoint corners[4];
    wxPoint padShapePos = ShapePos();       /* Note: for pad having a shape offset,
                                             * the pad position is NOT the shape position */
    switch( GetShape() )
    {
    case PAD_SHAPE_CIRCLE:
    case PAD_SHAPE_OVAL:
    case PAD_SHAPE_ROUNDRECT:
    case PAD_SHAPE_CHAMFERED_RECT:
    {
        // We are using TransformShapeWithClearanceToPolygon to build the shape.
        // Currently, this method uses only the same inflate value for X and Y dirs.
        // so because here this is not the case, we use a inflated dummy pad to build
        // the polygonal shape
        // TODO: remove this dummy pad when TransformShapeWithClearanceToPolygon will use
        // a wxSize to inflate the pad size
        D_PAD dummy( *this );
        dummy.SetSize( GetSize() + aInflateValue + aInflateValue );
        dummy.TransformShapeWithClearanceToPolygon( aCornerBuffer, 0 );
    }
        break;

    case PAD_SHAPE_TRAPEZOID:
    case PAD_SHAPE_RECT:
        aCornerBuffer.NewOutline();

        BuildPadPolygon( corners, aInflateValue, m_Orient );
        for( int ii = 0; ii < 4; ii++ )
        {
            corners[ii] += padShapePos;          // Shift origin to position
            aCornerBuffer.Append( corners[ii].x, corners[ii].y );
        }

        break;

    case PAD_SHAPE_CUSTOM:
        // for a custom shape, that is in fact a polygon (with holes), we can use only a inflate value.
        // so use ( aInflateValue.x + aInflateValue.y ) / 2 as polygon inflate value.
        // (different values for aInflateValue.x and aInflateValue.y has no sense for a custom pad)
        TransformShapeWithClearanceToPolygon(
                aCornerBuffer, ( aInflateValue.x + aInflateValue.y ) / 2 );
        break;
    }
}


bool D_PAD::BuildPadDrillShapePolygon(
        SHAPE_POLY_SET& aCornerBuffer, int aInflateValue, int aError ) const
{
    wxSize drillsize = GetDrillSize();

    if( !drillsize.x || !drillsize.y )
        return false;

    if( drillsize.x == drillsize.y )    // usual round hole
    {
        int radius = ( drillsize.x / 2 ) + aInflateValue;
        int numSegs = std::max( GetArcToSegmentCount( radius, aError, 360.0 ), 6 );
        TransformCircleToPolygon( aCornerBuffer, GetPosition(), radius, numSegs );
    }
    else    // Oblong hole
    {
        wxPoint start, end;
        int width;

        GetOblongDrillGeometry( start, end, width );

        width += aInflateValue * 2;
        int numSegs = std::max( GetArcToSegmentCount( width / 2, aError, 360.0 ), 6 );

        TransformRoundedEndsSegmentToPolygon(
                aCornerBuffer, GetPosition() + start, GetPosition() + end, numSegs, width );
    }

    return true;
}

/**
 * Function CreateThermalReliefPadPolygon
 * Add holes around a pad to create a thermal relief
 * copper thickness is min (dx/2, aCopperWitdh) or min (dy/2, aCopperWitdh)
 * @param aCornerBuffer = a buffer to store the polygon
 * @param aPad     = the current pad used to create the thermal shape
 * @param aThermalGap = gap in thermal shape
 * @param aCopperThickness = stubs thickness in thermal shape
 * @param aMinThicknessValue = min copper thickness allowed
 * @param aError = maximum error allowed when approximating arcs
 * @param aThermalRot = for rond pads the rotation of thermal stubs (450 usually for 45 deg.)
 */

/* thermal reliefs are created as 4 polygons.
 * each corner of a polygon if calculated for a pad at position 0, 0, orient 0,
 * and then moved and rotated acroding to the pad position and orientation
 */

/*
 * Note 1: polygons are drawm using outlines witk a thickness = aMinThicknessValue
 * so shapes must take in account this outline thickness
 *
 * Note 2:
 *      Trapezoidal pads are not considered here because they are very special case
 *      and are used in microwave applications and they *DO NOT* have a thermal relief that
 *      change the shape by creating stubs and destroy their properties.
 */
void CreateThermalReliefPadPolygon( SHAPE_POLY_SET& aCornerBuffer,
                                    const D_PAD&    aPad,
                                    int             aThermalGap,
                                    int             aCopperThickness,
                                    int             aMinThicknessValue,
                                    int             aError,
                                    double          aThermalRot )
{
    wxPoint corner, corner_end;
    wxSize  copper_thickness;
    wxPoint padShapePos = aPad.ShapePos();      // Note: for pad having a shape offset,
                                                // the pad position is NOT the shape position

    /* Keep in account the polygon outline thickness
     * aThermalGap must be increased by aMinThicknessValue/2 because drawing external outline
     * with a thickness of aMinThicknessValue will reduce gap by aMinThicknessValue/2
     */
    aThermalGap += aMinThicknessValue / 2;

    /* Keep in account the polygon outline thickness
     * copper_thickness must be decreased by aMinThicknessValue because drawing outlines
     * with a thickness of aMinThicknessValue will increase real thickness by aMinThicknessValue
     */
    int     dx = aPad.GetSize().x / 2;
    int     dy = aPad.GetSize().y / 2;

    copper_thickness.x = std::min( aPad.GetSize().x, aCopperThickness ) - aMinThicknessValue;
    copper_thickness.y = std::min( aPad.GetSize().y, aCopperThickness ) - aMinThicknessValue;

    if( copper_thickness.x < 0 )
        copper_thickness.x = 0;

    if( copper_thickness.y < 0 )
        copper_thickness.y = 0;

    switch( aPad.GetShape() )
    {
    case PAD_SHAPE_CIRCLE:    // Add 4 similar holes
        {
            /* we create 4 copper holes and put them in position 1, 2, 3 and 4
             * here is the area of the rectangular pad + its thermal gap
             * the 4 copper holes remove the copper in order to create the thermal gap
             * 4 ------ 1
             * |        |
             * |        |
             * |        |
             * |        |
             * 3 ------ 2
             * holes 2, 3, 4 are the same as hole 1, rotated 90, 180, 270 deg
             */

            // Build the hole pattern, for the hole in the X >0, Y > 0 plane:
            // The pattern roughtly is a 90 deg arc pie
            std::vector <wxPoint> corners_buffer;

            int    numSegs = std::max( GetArcToSegmentCount( dx + aThermalGap, aError, 360.0 ), 6 );
            double correction = GetCircletoPolyCorrectionFactor( numSegs );
            double delta = 3600.0 / numSegs;

            // Radius of outer arcs of the shape corrected for arc approximation by lines
            int outer_radius = KiROUND( ( dx + aThermalGap ) * correction );

            // Crosspoint of thermal spoke sides, the first point of polygon buffer
            corners_buffer.push_back( wxPoint( copper_thickness.x / 2, copper_thickness.y / 2 ) );

            // Add an intermediate point on spoke sides, to allow a > 90 deg angle between side
            // and first seg of arc approx
            corner.x = copper_thickness.x / 2;
            int y = outer_radius - (aThermalGap / 4);
            corner.y = KiROUND( sqrt( ( (double) y * y  - (double) corner.x * corner.x ) ) );

            if( aThermalRot != 0 )
                corners_buffer.push_back( corner );

            // calculate the starting point of the outter arc
            corner.x = copper_thickness.x / 2;

            corner.y = KiROUND( sqrt( ( (double) outer_radius * outer_radius ) -
                                      ( (double) corner.x * corner.x ) ) );
            RotatePoint( &corner, 90 ); // 9 degrees is the spoke fillet size

            // calculate the ending point of the outer arc
            corner_end.x = corner.y;
            corner_end.y = corner.x;

            // calculate intermediate points (y coordinate from corner.y to corner_end.y
            while( (corner.y > corner_end.y)  && (corner.x < corner_end.x) )
            {
                corners_buffer.push_back( corner );
                RotatePoint( &corner, delta );
            }

            corners_buffer.push_back( corner_end );

            /* add an intermediate point, to avoid angles < 90 deg between last arc approx line
             * and radius line
             */
            corner.x = corners_buffer[1].y;
            corner.y = corners_buffer[1].x;
            corners_buffer.push_back( corner );

            // Now, add the 4 holes ( each is the pattern, rotated by 0, 90, 180 and 270  deg
            // aThermalRot = 450 (45.0 degrees orientation) work fine.
            double angle_pad = aPad.GetOrientation();              // Pad orientation
            double th_angle  = aThermalRot;

            for( unsigned ihole = 0; ihole < 4; ihole++ )
            {
                aCornerBuffer.NewOutline();

                for( unsigned ii = 0; ii < corners_buffer.size(); ii++ )
                {
                    corner = corners_buffer[ii];
                    RotatePoint( &corner, th_angle + angle_pad );          // Rotate by segment angle and pad orientation
                    corner += padShapePos;
                    aCornerBuffer.Append( corner.x, corner.y );
                }

                th_angle += 900;       // Note: th_angle in in 0.1 deg.
            }
        }
        break;

    case PAD_SHAPE_OVAL:
        {
            // Oval pad support along the lines of round and rectangular pads
            std::vector <wxPoint> corners_buffer;               // Polygon buffer as vector

            dx = (aPad.GetSize().x / 2) + aThermalGap;     // Cutout radius x
            dy = (aPad.GetSize().y / 2) + aThermalGap;     // Cutout radius y

            wxPoint shape_offset;

            // We want to calculate an oval shape with dx > dy.
            // if this is not the case, exchange dx and dy, and rotate the shape 90 deg.
            int supp_angle = 0;

            if( dx < dy )
            {
                std::swap( dx, dy );
                supp_angle = 900;
                std::swap( copper_thickness.x, copper_thickness.y );
            }

            int deltasize = dx - dy;        // = distance between shape position and the 2 demi-circle ends centre
            // here we have dx > dy
            // Radius of outer arcs of the shape:
            int outer_radius = dy;     // The radius of the outer arc is radius end + aThermalGap


            int    numSegs = std::max( GetArcToSegmentCount( outer_radius, aError, 360.0 ), 6 );
            double delta = 3600.0 / numSegs;

            // Some coordinate fiddling, depending on the shape offset direction
            shape_offset = wxPoint( deltasize, 0 );

            // Crosspoint of thermal spoke sides, the first point of polygon buffer
            corner.x = copper_thickness.x / 2;
            corner.y = copper_thickness.y / 2;
            corners_buffer.push_back( corner );

            // Arc start point calculation, the intersecting point of cutout arc and thermal spoke edge
            // If copper thickness is more than shape offset, we need to calculate arc intercept point.
            if( copper_thickness.x > deltasize )
            {
                corner.x = copper_thickness.x / 2;
                corner.y = KiROUND( sqrt( ( (double) outer_radius * outer_radius ) -
                                        ( (double) ( corner.x - delta ) * ( corner.x - deltasize ) ) ) );
                corner.x -= deltasize;

                /* creates an intermediate point, to have a > 90 deg angle
                 * between the side and the first segment of arc approximation
                 */
                wxPoint intpoint = corner;
                intpoint.y -= aThermalGap / 4;
                corners_buffer.push_back( intpoint + shape_offset );
                RotatePoint( &corner, 90 ); // 9 degrees of thermal fillet
            }
            else
            {
                corner.x = copper_thickness.x / 2;
                corner.y = outer_radius;
                corners_buffer.push_back( corner );
            }

            // Add an intermediate point on spoke sides, to allow a > 90 deg angle between side
            // and first seg of arc approx
            wxPoint last_corner;
            last_corner.y = copper_thickness.y / 2;
            int     px = outer_radius - (aThermalGap / 4);
            last_corner.x =
                KiROUND( sqrt( ( ( (double) px * px ) - (double) last_corner.y * last_corner.y ) ) );

            // Arc stop point calculation, the intersecting point of cutout arc and thermal spoke edge
            corner_end.y = copper_thickness.y / 2;
            corner_end.x =
                KiROUND( sqrt( ( (double) outer_radius *
                             outer_radius ) - ( (double) corner_end.y * corner_end.y ) ) );
            RotatePoint( &corner_end, -90 ); // 9 degrees of thermal fillet

            // calculate intermediate arc points till limit is reached
            while( (corner.y > corner_end.y)  && (corner.x < corner_end.x) )
            {
                corners_buffer.push_back( corner + shape_offset );
                RotatePoint( &corner, delta );
            }

            //corners_buffer.push_back(corner + shape_offset);      // TODO: about one mil geometry error forms somewhere.
            corners_buffer.push_back( corner_end + shape_offset );
            corners_buffer.push_back( last_corner + shape_offset );         // Enabling the line above shows intersection point.

            /* Create 2 holes, rotated by pad rotation.
             */
            double angle = aPad.GetOrientation() + supp_angle;

            for( int irect = 0; irect < 2; irect++ )
            {
                aCornerBuffer.NewOutline();
                for( unsigned ic = 0; ic < corners_buffer.size(); ic++ )
                {
                    wxPoint cpos = corners_buffer[ic];
                    RotatePoint( &cpos, angle );
                    cpos += padShapePos;
                    aCornerBuffer.Append( cpos.x, cpos.y );
                }

                angle = AddAngles( angle, 1800 ); // this is calculate hole 3
            }

            // Create holes, that are the mirrored from the previous holes
            for( unsigned ic = 0; ic < corners_buffer.size(); ic++ )
            {
                wxPoint swap = corners_buffer[ic];
                swap.x = -swap.x;
                corners_buffer[ic] = swap;
            }

            // Now add corner 4 and 2 (2 is the corner 4 rotated by 180 deg
            angle = aPad.GetOrientation() + supp_angle;

            for( int irect = 0; irect < 2; irect++ )
            {
                aCornerBuffer.NewOutline();

                for( unsigned ic = 0; ic < corners_buffer.size(); ic++ )
                {
                    wxPoint cpos = corners_buffer[ic];
                    RotatePoint( &cpos, angle );
                    cpos += padShapePos;
                    aCornerBuffer.Append( cpos.x, cpos.y );
                }

                angle = AddAngles( angle, 1800 );
            }
        }
        break;

    case PAD_SHAPE_CHAMFERED_RECT:
    case PAD_SHAPE_ROUNDRECT:   // thermal shape is the same for rectangular shapes.
    case PAD_SHAPE_RECT:
        {
            /* we create 4 copper holes and put them in position 1, 2, 3 and 4
             * here is the area of the rectangular pad + its thermal gap
             * the 4 copper holes remove the copper in order to create the thermal gap
             * 1 ------ 4
             * |        |
             * |        |
             * |        |
             * |        |
             * 2 ------ 3
             * hole 3 is the same as hole 1, rotated 180 deg
             * hole 4 is the same as hole 2, rotated 180 deg and is the same as hole 1, mirrored
             */

            // First, create a rectangular hole for position 1 :
            // 2 ------- 3
            //  |        |
            //  |        |
            //  |        |
            // 1  -------4

            // Modified rectangles with one corner rounded. TODO: merging with oval thermals
            // and possibly round too.

            std::vector <wxPoint> corners_buffer;               // Polygon buffer as vector

            dx = (aPad.GetSize().x / 2) + aThermalGap;         // Cutout radius x
            dy = (aPad.GetSize().y / 2) + aThermalGap;         // Cutout radius y

            // calculation is optimized for pad shape with dy >= dx (vertical rectangle).
            // if it is not the case, just rotate this shape 90 degrees:
            double angle = aPad.GetOrientation();
            wxPoint corner_origin_pos( -aPad.GetSize().x / 2, -aPad.GetSize().y / 2 );

            if( dy < dx )
            {
                std::swap( dx, dy );
                std::swap( copper_thickness.x, copper_thickness.y );
                std::swap( corner_origin_pos.x, corner_origin_pos.y );
                angle += 900.0;
            }
            // Now calculate the hole pattern in position 1 ( top left pad corner )

            // The first point of polygon buffer is left lower corner, second the crosspoint of
            // thermal spoke sides, the third is upper right corner and the rest are rounding
            // vertices going anticlockwise. Note the inverted Y-axis in corners_buffer y coordinates.
            wxPoint arc_end_point( -dx, -(aThermalGap / 4 + copper_thickness.y / 2) );
            corners_buffer.push_back( arc_end_point );          // Adds small miters to zone
            corners_buffer.push_back( wxPoint( -(dx - aThermalGap / 4), -copper_thickness.y / 2 ) );    // fill and spoke corner
            corners_buffer.push_back( wxPoint( -copper_thickness.x / 2, -copper_thickness.y / 2 ) );
            corners_buffer.push_back( wxPoint( -copper_thickness.x / 2, -(dy - aThermalGap / 4) ) );
            // The first point to build the rounded corner:
            wxPoint arc_start_point( -(aThermalGap / 4 + copper_thickness.x / 2) , -dy );
            corners_buffer.push_back( arc_start_point );

            int    numSegs = std::max( GetArcToSegmentCount( aThermalGap, aError, 360.0 ), 6 );
            double correction = GetCircletoPolyCorrectionFactor( numSegs );
            int    rounding_radius = KiROUND( aThermalGap * correction ); // Corner rounding radius

            // Calculate arc angle parameters.
            // the start angle id near 900 decidegrees, the final angle is near 1800.0 decidegrees.
            double arc_increment = 3600.0 / numSegs;

            // the arc_angle_start is 900.0 or slighly more, depending on the actual arc starting point
            double arc_angle_start = atan2( -arc_start_point.y -corner_origin_pos.y, arc_start_point.x - corner_origin_pos.x ) * 1800/M_PI;
            if( arc_angle_start < 900.0 )
                arc_angle_start = 900.0;

            bool first_point = true;
            for( double curr_angle = arc_angle_start; ; curr_angle += arc_increment )
            {
                wxPoint corner_position = wxPoint( rounding_radius, 0 );
                RotatePoint( &corner_position, curr_angle );        // Rounding vector rotation
                corner_position += corner_origin_pos;               // Rounding vector + Pad corner offset

                // The arc angle is <= 90 degrees, therefore the arc is finished if the x coordinate
                // decrease or the y coordinate is smaller than the y end point
                if( !first_point &&
                    ( corner_position.x >= corners_buffer.back().x || corner_position.y > arc_end_point.y ) )
                    break;

                first_point = false;

                // Note: for hole in position 1, arc x coordinate is always < x starting point
                // and arc y coordinate is always <= y ending point
                if( corner_position != corners_buffer.back()    // avoid duplicate corners.
                    && corner_position.x <= arc_start_point.x )   // skip current point at the right of the starting point
                    corners_buffer.push_back( corner_position );
            }

            for( int irect = 0; irect < 2; irect++ )
            {
                aCornerBuffer.NewOutline();

                for( unsigned ic = 0; ic < corners_buffer.size(); ic++ )
                {
                    wxPoint cpos = corners_buffer[ic];
                    RotatePoint( &cpos, angle );            // Rotate according to module orientation
                    cpos += padShapePos;                    // Shift origin to position
                    aCornerBuffer.Append( cpos.x, cpos.y );
                }

                angle = AddAngles( angle, 1800 );       // this is calculate hole 3
            }

            // Create holes, that are the mirrored from the previous holes
            for( unsigned ic = 0; ic < corners_buffer.size(); ic++ )
            {
                wxPoint swap = corners_buffer[ic];
                swap.x = -swap.x;
                corners_buffer[ic] = swap;
            }

            // Now add corner 4 and 2 (2 is the corner 4 rotated by 180 deg
            for( int irect = 0; irect < 2; irect++ )
            {
                aCornerBuffer.NewOutline();

                for( unsigned ic = 0; ic < corners_buffer.size(); ic++ )
                {
                    wxPoint cpos = corners_buffer[ic];
                    RotatePoint( &cpos, angle );
                    cpos += padShapePos;
                    aCornerBuffer.Append( cpos.x, cpos.y );
                }

                angle = AddAngles( angle, 1800 );
            }
        }
        break;

    case PAD_SHAPE_TRAPEZOID:
        {
        SHAPE_POLY_SET antipad;       // The full antipad area

        // We need a length to build the stubs of the thermal reliefs
        // the value is not very important. The pad bounding box gives a reasonable value
        EDA_RECT bbox = aPad.GetBoundingBox();
        int stub_len = std::max( bbox.GetWidth(), bbox.GetHeight() );

        aPad.TransformShapeWithClearanceToPolygon( antipad, aThermalGap );

        SHAPE_POLY_SET stub;          // A basic stub ( a rectangle)
        SHAPE_POLY_SET stubs;        // the full stubs shape


        // We now substract the stubs (connections to the copper zone)
        //ClipperLib::Clipper clip_engine;
        // Prepare a clipping transform
        //clip_engine.AddPath( antipad, ClipperLib::ptSubject, true );

        // Create stubs and add them to clipper engine
        wxPoint stubBuffer[4];
        stubBuffer[0].x = stub_len;
        stubBuffer[0].y = copper_thickness.y/2;
        stubBuffer[1] = stubBuffer[0];
        stubBuffer[1].y = -copper_thickness.y/2;
        stubBuffer[2] = stubBuffer[1];
        stubBuffer[2].x = -stub_len;
        stubBuffer[3] = stubBuffer[2];
        stubBuffer[3].y = copper_thickness.y/2;

        stub.NewOutline();

        for( unsigned ii = 0; ii < arrayDim( stubBuffer ); ii++ )
        {
            wxPoint cpos = stubBuffer[ii];
            RotatePoint( &cpos, aPad.GetOrientation() );
            cpos += padShapePos;
            stub.Append( cpos.x, cpos.y );
        }

        stubs.Append( stub );

        stubBuffer[0].y = stub_len;
        stubBuffer[0].x = copper_thickness.x/2;
        stubBuffer[1] = stubBuffer[0];
        stubBuffer[1].x = -copper_thickness.x/2;
        stubBuffer[2] = stubBuffer[1];
        stubBuffer[2].y = -stub_len;
        stubBuffer[3] = stubBuffer[2];
        stubBuffer[3].x = copper_thickness.x/2;

        stub.RemoveAllContours();
        stub.NewOutline();

        for( unsigned ii = 0; ii < arrayDim( stubBuffer ); ii++ )
        {
            wxPoint cpos = stubBuffer[ii];
            RotatePoint( &cpos, aPad.GetOrientation() );
            cpos += padShapePos;
            stub.Append( cpos.x, cpos.y );
        }

        stubs.Append( stub );
        stubs.Simplify( SHAPE_POLY_SET::PM_FAST );

        antipad.BooleanSubtract( stubs, SHAPE_POLY_SET::PM_FAST );
        aCornerBuffer.Append( antipad );

        break;
        }

    default:
        ;
    }
}

void ZONE_CONTAINER::TransformShapeWithClearanceToPolygon(
        SHAPE_POLY_SET& aCornerBuffer, int aClearanceValue, int aError, bool ignoreLineWidth ) const
{
    wxASSERT_MSG( !ignoreLineWidth, "IgnoreLineWidth has no meaning for zones." );

    aCornerBuffer = m_FilledPolysList;
    aCornerBuffer.Simplify( SHAPE_POLY_SET::PM_STRICTLY_SIMPLE );
}
