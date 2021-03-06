/*****************************************************************************
 *                                                                           *
 * The breve Simulation Environment                                          *
 * Copyright (C) 2000, 2001, 2002, 2003 Jonathan Klein                       *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *****************************************************************************/

#include "simulation.h"
#include "glIncludes.h"

#include "world.h"
#include "camera.h"
#include "shadowvolume.h"
#include "shape.h"

void slShape::drawShadowVolume( slCamera *c, slPosition *p ) {
	std::vector< slEdge* >::iterator ei;
	slVector light;
	slVector lNormal;

#if 0
	slVectorCopy( &c->_lights[ 0 ]._location, &light );
	slVectorCopy( &light, &lNormal );
	slVectorNormalize( &lNormal );
	slVectorMul( &light, 5, &light );

	glBegin( GL_QUADS );

	for ( ei = edges.begin(); ei != edges.end(); ei++ ) {
		slEdge *e = *ei;
		double d1, d2;
		slFace *f1, *f2;
		slVector n1, n2;
		slVector tv;

		f1 = e->faces[0];
		f2 = e->faces[1];

		// look at this edge's faces, and calculate the dot product
		// with the light's vector.

		slMatrix t;

		slMatrixMulMatrix( p -> rotation, _transform, t );

		slVectorXform( t, &f1->plane.normal, &n1 );
		slVectorXform( t, &f2->plane.normal, &n2 );

		d1 = slVectorDot( &n1, &lNormal );
		d2 = slVectorDot( &n2, &lNormal );

		// are we at a critical edge, in which one face is facing
		// towards the light, and the other is facing away?

		if ( d1 * d2 < 0.0 || ( d1 * d2 == 0.0 && ( d1 + d2 ) > 0.0 ) ) {
			slVector ts, te, sBottom, eBottom;
			slFace *topFace;
			int n;
			int flip = 0;

			if ( d1 > 0.0 ) topFace = f1;
			else topFace = f2;

			for ( n = 0;n < topFace -> _pointCount;n++ ) {
				if ( topFace -> neighbors[n] == e ) {
					if ( e->neighbors[0] == topFace->points[n] ) 
						flip = 1;

					n = topFace -> _pointCount;
				}
			}

			slVector *v1, *v2;

			if ( !flip ) {
				v1 = &(( slPoint* )e->neighbors[0] )->vertex;
				v2 = &(( slPoint* )e->neighbors[1] )->vertex;
			} else {
				v1 = &(( slPoint* )e->neighbors[1] )->vertex;
				v2 = &(( slPoint* )e->neighbors[0] )->vertex;
			}

			slVectorMul( v1, 1.01, &ts );
			slVectorXform( _transform, &ts, &tv );
			slPositionVertex( p, &tv, &ts );

			slVectorMul( v2, 1.01, &te );
			slVectorXform( _transform, &te, &tv );
			slPositionVertex( p, &tv, &te );


			slVectorSub( &ts, &light, &sBottom );

			slVectorSub( &te, &light, &eBottom );

			glVertex3f( te.x, te.y, te.z );
			glVertex3f( eBottom.x, eBottom.y, eBottom.z );
			glVertex3f( sBottom.x, sBottom.y, sBottom.z );
			glVertex3f( ts.x, ts.y, ts.z );
		}
	}

	glEnd();
#endif
}

void slSphere::drawShadowVolume( slCamera *c, slPosition *p ) {

#if 0
	slVector light, lNormal, x1, x2, lastV;
	int n;
	double diff;
	int divisions;

	static int inited;
	static float sineTable[361], cosineTable[361];

	slVectorSub( &c->_lights[0]._location, &p->location, &light );

	// slVectorCopy(&c->_lights[0]._location, &light);

	slVectorCopy( &light, &lNormal );

	slVectorNormalize( &lNormal );

	slVectorMul( &light, 5, &light );

	// we want two vectors perpendicular to lNormal
	// make a phony vector, find  phony x light

	if ( lNormal.x != lNormal.y ) slVectorSet( &x2, lNormal.y, lNormal.x, lNormal.z );
	else slVectorSet( &x2, lNormal.x, lNormal.z, lNormal.y );

	slVectorCross( &lNormal, &x2, &x1 );
	slVectorCross( &lNormal, &x1, &x2 );

	slVectorNormalize( &x1 );
	slVectorNormalize( &x2 );

	divisions = ( int )( _radius * 5.0 );

	if ( divisions < MIN_SPHERE_VOLUME_DIVISIONS ) 
		divisions = MIN_SPHERE_VOLUME_DIVISIONS;
	else if ( divisions > MAX_SPHERE_VOLUME_DIVISIONS ) 
		divisions = MAX_SPHERE_VOLUME_DIVISIONS;

	if ( !inited ) {
		double step = ( 2.0 * M_PI / 360.0 );
		inited = 1;

		for ( n = 0; n < 361;n++ ) {
			sineTable[ n] = sin( step * n ) * 1.05;
			cosineTable[ n] = cos( step * n ) * 1.05;
		}
	}

	diff = 2.0 * M_PI / ( double )divisions;

	double step = ( 360.0f / divisions );

	glBegin( GL_QUADS );

	for ( n = 0; n < divisions + 1; n++ ) {
		slVector dx, dy, v, vBottom, lBottom;

		// make the last index line up with the first -- otherwise, rounding 
		// error can make it so that the rays don't line up.

		int index = ( n == divisions ) ? 0 : (int)( n * step );

		slVectorMul( &x1, _radius * cosineTable[ index ], &dx );
		slVectorMul( &x2, _radius * sineTable[   index ], &dy );

		slVectorAdd( &p->location, &dx, &v );
		slVectorAdd( &v, &dy, &v );

		if ( n != 0 ) {
			slVectorSub( &v, &light, &vBottom );
			slVectorSub( &lastV, &light, &lBottom );

			glVertex3f( lastV.x, lastV.y, lastV.z );
			glVertex3f( lBottom.x, lBottom.y, lBottom.z );
			glVertex3f( vBottom.x, vBottom.y, vBottom.z );
			glVertex3f( v.x, v.y, v.z );
		}

		slVectorCopy( &v, &lastV );
	}

	glEnd();

	glBegin( GL_POLYGON );

	for ( n = 0;n < divisions + 1;n++ ) {
		slVector dx, dy, v;

		int index = ( n == divisions ) ? 0 : (int)( n * step );
		slVectorMul( &x1, _radius * cosineTable[  index ], &dx );
		slVectorMul( &x2, _radius * sineTable[    index ], &dy );

		slVectorAdd( &p->location, &dx, &v );
		slVectorAdd( &v, &dy, &v );

		glVertex3f( v.x, v.y, v.z );
	}

	glEnd();
	
#endif
}

void slCamera::renderShadowVolume( slWorld *w ) {
#if 0
	setupLights( 0 );

	glClear( GL_STENCIL_BUFFER_BIT );

	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_FALSE );

	// glEnable(GL_POLYGON_OFFSET_FILL);

	glEnable( GL_STENCIL_TEST );
	glEnable( GL_CULL_FACE );
	glCullFace( GL_BACK );
	glStencilFunc( GL_ALWAYS, 1, 0xffffffff );
	glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );

	// stencil up shadow volume front faces to 1
	renderObjectShadowVolumes( w );

	// stencil down shadow volume back faces to 0

	glStencilOp( GL_KEEP, GL_KEEP, GL_DECR );
	glCullFace( GL_FRONT );
	renderObjectShadowVolumes( w );

	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glDepthMask( GL_TRUE );
	glCullFace( GL_BACK );
	glDepthFunc( GL_LEQUAL );
	glStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );

	glStencilFunc( GL_EQUAL, 0, 0xffffffff );

	// draw the scene again, with lighting, only where the value is 0

	// glColor3f(1, 0, 0);
	renderObjects( w, DO_NO_ALPHA | DO_NO_SHADOWCATCHER );
	glEnable( GL_BLEND );
	renderObjects( w, DO_ONLY_SHADOWCATCHER, .65 );

	// transparent objects cause problems, since they cannot simply
	// be "drawn over" the way we do with the rest of the scene.
	// once we've drawn it the first time, it's there to stay.
	// what I think we should do here:
	// 1) do not render the alphas at all for the first pass
	// 2) render the unlit alphas where the stencil != 0

	renderObjects( w, DO_ONLY_ALPHA );

	if ( _billboardCount ) 
		renderBillboards( 0 );

	glDisable( GL_STENCIL_TEST );
	
#endif
}

void slCamera::renderObjectShadowVolumes( slWorld *w ) {
	std::vector<slWorldObject*>::iterator wi;

	glDisable( GL_BLEND );
	glColor4f( 0, 0, 0, 1.0 );

	for ( wi = w->_objects.begin(); wi != w->_objects.end(); wi++ ) {
		if ( *wi && ( *wi )->_displayShape && !( *wi )->_drawAsPoint && ( *wi )->_drawShadow ) {
			( *wi )->_displayShape->drawShadowVolume( this, &( *wi )->_position );
		}
	}
}
