#include <agge/curves.h>

#include "assertex.h"
#include "mocks.h"

#include <agge/path.h>

#include <ut/test.h>

namespace agge
{
	namespace tests
	{
		begin_test_suite( QBezierCurveTests )
			test( QBezier2SegmentProducesIterator )
			{
				// INIT
				real_t x = 11.0f, y = 13.0f;
				const qbezier b(0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 0.0f);

				// ACT
				qbezier::iterator i = b.iterate();

				// ACT / ASSERT
				assert_equal(path_command_move_to, i.vertex(&x, &y));
				assert_equal(0.0f, x);
				assert_equal(0.0f, y);
			}


			test( CoarseQBezier2ProducesLine )
			{
				// INIT
				const qbezier b1(0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 0.0f, 100.0f /*longest segment*/);
				qbezier::iterator i = b1.iterate();

				// ACT
				mocks::path::point points1[] = { vertex(i), vertex(i), };

				// ASSERT
				mocks::path::point reference1[] = { moveto(0.0f, 0.0f), lineto(2.0f, 0.0f), };

				assert_equal(reference1, points1);

				// INIT
				const qbezier b2(10.0f, 11.0f, 1.0f, 1.0f, 13.0f, 17.0f, 100.0f /*longest segment*/);
				
				i = b2.iterate();

				// ACT
				mocks::path::point points2[] = { vertex(i), vertex(i), };

				// ASSERT
				mocks::path::point reference2[] = { moveto(10.0f, 11.0f), lineto(13.0f, 17.0f), };

				assert_equal(reference2, points2);
			}


			test( DegenrateQBezierLengthIsApproximatedToTheStartEndSegmentLength )
			{
				// INIT
				const qbezier b1(-1.0f, 0.0f, 0.5f, 0.25f, 2.0f, 0.5f);

				// ACT / ASSERT
				assert_equal_approx(3.041381f, b1.approximate_length(), 5);

				// INIT
				const qbezier b2(-3.0f, -4.0f, -0.3f, -0.4f, 0.0f, 0.0f);

				// ACT / ASSERT
				assert_equal_approx(5.0f, b2.approximate_length(), 5);
			}


			test( QBezierLengthIsApproximatedToAverageOfStartEndSegmentAndStartCenterCenterEndSegments )
			{
				// INIT
				const qbezier b1(-1.0f, 0.0f, 0.0f, 0.0f, 3.0f, 4.0f);

				// ACT / ASSERT
				assert_equal_approx(5.8284f, b1.approximate_length(), 5);

				// INIT
				const qbezier b2(-1.0f, 2.0f, 3.0f, 5.0f, 7.0f, 13.0f); // 0.5 * (13.6014705 + 5 + 8.9442719)

				// ACT / ASSERT
				assert_equal_approx(13.7728, b2.approximate_length(), 5);
			}


			test( QBezierIteratorProducesNumberOfVerticesAccordinglyToStep )
			{
				// INIT
				real_t x, y;
				qbezier::iterator i1(0.0f, 0.0f, 0.5f, 0.6f, 1.0f, 2.0f, 0.5f);

				// ACT / ASSERT
				assert_equal(path_command_move_to, i1.vertex(&x, &y));
				assert_equal(path_command_line_to, i1.vertex(&x, &y));
				assert_equal(path_command_line_to, i1.vertex(&x, &y));
				assert_equal(path_command_stop, i1.vertex(&x, &y));

				// INIT
				qbezier::iterator i2(0.0f, 0.0f, 0.5f, 0.6f, 1.0f, 2.0f, 0.2f);

				// ACT / ASSERT
				assert_equal(path_command_move_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_stop, i2.vertex(&x, &y));
			}


			test( QBezierIteratorProducesExpectedVerticesAccordinglyToParameters )
			{
				// INIT
				qbezier::iterator i1(-3.5f, 7.4f, 1.0f, 2.0f, 10.1f, 3.8f, 0.111f);

				// ACT
				mocks::path::point points1[] = {
					vertex(i1), vertex(i1), vertex(i1), vertex(i1), vertex(i1),
					vertex(i1), vertex(i1), vertex(i1), vertex(i1), vertex(i1),
					vertex(i1),
				};

				// ASSERT
				mocks::path::point reference1[] = {
					moveto(-3.50000000f, 7.40000010f),
					lineto(-2.44432354f, 6.28991079f),
					lineto(-1.27529359f, 5.35724497f),
					lineto(0.00708937645f, 4.60200071f),
					lineto(1.40282583f, 4.02417898f),
					lineto(2.91191530f, 3.62378001f),
					lineto(4.53435755f, 3.40080309f),
					lineto(6.27015400f, 3.35524869f),
					lineto(8.11930275f, 3.48711658f),
					lineto(10.0818052f, 3.79640722f),
					lineto(10.1000004f, 3.79999995f),
				};

				assert_equal(reference1, points1);

				// INIT
				qbezier::iterator i2(1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.3f);

				// ACT
				mocks::path::point points2[] = { vertex(i2), vertex(i2), vertex(i2), vertex(i2), vertex(i2), };

				// ASSERT
				mocks::path::point reference2[] = {
					moveto(1.00000000f, 1.00000000f),
					lineto(0.399999976f, 0.579999983f),
					lineto(-0.200000033f, 0.519999981f),
					lineto(-0.800000072f, 0.820000052f),
					lineto(-1.00000000f, 1.00000000f),
				};

				assert_equal(reference2, points2);
			}

		end_test_suite


		begin_test_suite( CBezierCurveTests )
			test( CBezierIteratorProducesNumberOfVerticesAccordinglyToStep )
			{
				// INIT
				real_t x, y;
				cbezier::iterator i1(0.0f, 0.0f, 0.5f, 0.6f, 0.0f, 0.0f, 1.0f, 2.0f, 0.5f);

				// ACT / ASSERT
				assert_equal(path_command_move_to, i1.vertex(&x, &y));
				assert_equal(path_command_line_to, i1.vertex(&x, &y));
				assert_equal(path_command_line_to, i1.vertex(&x, &y));
				assert_equal(path_command_stop, i1.vertex(&x, &y));

				// INIT
				cbezier::iterator i2(0.0f, 0.0f, 0.5f, 0.6f, 0.0f, 0.0f, 1.0f, 2.0f, 0.2f);

				// ACT / ASSERT
				assert_equal(path_command_move_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_line_to, i2.vertex(&x, &y));
				assert_equal(path_command_stop, i2.vertex(&x, &y));
			}


			test( QBezierIteratorProducesExpectedVerticesAccordinglyToParameters )
			{
				// INIT
				cbezier::iterator i1(-3.5f, 7.4f, 1.0f, 2.0f, 0.2f, 0.3f, 10.1f, 3.8f, 0.111f);

				// ACT
				mocks::path::point points1[] = {
					vertex(i1), vertex(i1), vertex(i1), vertex(i1), vertex(i1),
					vertex(i1), vertex(i1), vertex(i1), vertex(i1), vertex(i1),
					vertex(i1),
				};

				// ASSERT
				mocks::path::point reference1[] = {
					moveto(-3.50000000f, 7.40000010f),
					lineto(-2.17552185f, 5.74061489f),
					lineto(-1.11155891f, 4.36706400f),
					lineto(-0.176818520f, 3.2916567f),
					lineto(0.759991825f, 2.52670193f),
					lineto(1.83016467f, 2.08450842f),
					lineto(3.16499233f, 1.97738409f),
					lineto(4.89576817f, 2.21763802f),
					lineto(7.15378380f, 2.81757903f),
					lineto(10.0703335f, 3.78951573f),
					lineto(10.1000004f, 3.79999995f),
				};

				assert_equal(reference1, points1);

				// INIT
				cbezier::iterator i2(1.0f, 1.0f, 0.0f, 0.0f, 0.8f, 0.7f, -1.0f, 1.0f, 0.3f);

				// ACT
				mocks::path::point points2[] = { vertex(i2), vertex(i2), vertex(i2), vertex(i2), vertex(i2), };

				// ASSERT
				mocks::path::point reference2[] = {
					moveto(1.00000000f, 1.00000000f),
					lineto(0.467199981f, 0.502300024f),
					lineto(0.193599969f, 0.582399964f),
					lineto(-0.533600152f, 0.900100052f),
					lineto(-1.00000000f, 1.00000000f),
				};

				assert_equal(reference2, points2);
			}

		end_test_suite
	}
}
