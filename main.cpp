#include <thread>
#include <time.h>
#include <sys/time.h>
#include "main_config.h"
#include "quadcopter.h"

#ifdef MAIN_RENDER_TYPE_IRRLICHT
#include "irr_renderer.h"
#elif SECOND_RENDERER
// another renderer
#endif

int main(int argc, char *argv[])
{
	// create simulation in second thread
	quadcopter Quadcopter_sim;
	Quadcopter_sim.startSimulation();

	// create the renderer
#ifdef MAIN_RENDER_TYPE_IRRLICHT
	irr_renderer IRR_Renderer(Quadcopter_sim.get_frame_mode(), MAIN_RENDERER_WINDOWS_X_SIZE, MAIN_RENDERER_WINDOWS_Y_SIZE, MAIN_RENDERER_FRAMES_SEC);
#elif SECOND_RENDERER
	// another renderer
#endif

	// infinite execution loop
	while(1)
	{
		// check if both simulation and renderer are still running
#ifdef MAIN_RENDER_TYPE_IRRLICHT
		if(Quadcopter_sim.simulationRunning() && IRR_Renderer.get_rendererRunning())
		{
#elif SECOND_RENDERER
		// another renderer
#else
		// or just the simulation if no renderer is used
		if(Quadcopter_sim.simulationRunning())
		{
#endif
			// frame rate limiter start
			timeval start, finish;
			gettimeofday(&start, NULL);

			// render the scene
#ifdef MAIN_RENDER_TYPE_IRRLICHT
			// extract data from simulation and pass it to the irrlicht renderer
			IRR_Renderer.Quadcopter_3D->set_position(Quadcopter_sim.get_position(0), Quadcopter_sim.get_position(1), Quadcopter_sim.get_position(2));
			IRR_Renderer.Quadcopter_3D->set_attitude(Quadcopter_sim.get_attitude(0), Quadcopter_sim.get_attitude(1), Quadcopter_sim.get_attitude(2));
			IRR_Renderer.Quadcopter_3D->set_speed(Quadcopter_sim.get_speed(0), Quadcopter_sim.get_speed(1), Quadcopter_sim.get_speed(2));
			IRR_Renderer.Quadcopter_3D->set_motorspeed(	Quadcopter_sim.get_motor_rpm(0), Quadcopter_sim.get_motor_rpm(1), Quadcopter_sim.get_motor_rpm(2), Quadcopter_sim.get_motor_rpm(3));
			IRR_Renderer.Quadcopter_3D->set_direction_vector(Quadcopter_sim.get_direction_vector(0), Quadcopter_sim.get_direction_vector(1), Quadcopter_sim.get_direction_vector(2));
			IRR_Renderer.Quadcopter_3D->set_up_vector(Quadcopter_sim.get_up_vector(0), Quadcopter_sim.get_up_vector(1), Quadcopter_sim.get_up_vector(2));
			IRR_Renderer.render();
#elif SECOND_RENDERER
			// another renderer
#endif
			// frame rate limiter end
			gettimeofday(&finish, NULL);
			unsigned int execution_time_is_us = (unsigned int) ((finish.tv_sec - start.tv_sec)/1000000);
			unsigned int executions_time_target_us = 1000000/MAIN_RENDERER_FRAMES_SEC;
			if(executions_time_target_us > execution_time_is_us)
			{
				unsigned int time_difference_us = executions_time_target_us - execution_time_is_us;
				std::this_thread::sleep_for(std::chrono::microseconds(time_difference_us));
			}
		}
		// leave if renderer of simulation has stopped
		else
		{
			// close simulation thread savely
			Quadcopter_sim.stopSimulation();
			while(Quadcopter_sim.simulationRunning() == true)
				std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_NO_RENDERER_SLEEP));

			// close renderer savely
#ifdef MAIN_RENDER_TYPE_IRRLICHT
			IRR_Renderer.~irr_renderer();
#elif SECOND_RENDERER
			// another renderer
#endif
			// leave infinite execution loop
			break;
		}
	}

	return 0;
}
