//
// Created by amr on 04/07/2020.
//
#include "read_utils.h"
#include <skeleton/helpers/memory_allocation/memory_allocator.h>

#include <unordered_set>

void ParseSeioToTraces(SeIO *sio, Point3D *source, Traces *traces, uint **x_position, uint **y_position,
        GridBox *grid, ComputationParameters *parameters, float *total_time) {
    std::unordered_set<uint> x_dim;
    std::unordered_set<uint> y_dim;
    float scale = abs(sio->Atraces.at(0).TraceMetaData.scalar) * 1.0;
    // Get source point.
    source->x = (sio->Atraces.at(0).TraceMetaData.source_location_x - (int)grid->reference_point.x) /
                           (grid->cell_dimensions.dx * scale);
    source->z = (sio->Atraces.at(0).TraceMetaData.source_location_z - (int)grid->reference_point.z) /
                           (grid->cell_dimensions.dz * scale);
    source->y = (sio->Atraces.at(0).TraceMetaData.source_location_y - (int)grid->reference_point.y) /
                           (grid->cell_dimensions.dy * scale);
    // If window model, need to setup the starting point of the window and adjust source point.
    // Handle 3 cases : no room for left window, no room for right window, room for both.
    // Those 3 cases can apply to y-direction as well if 3D.
    if (parameters->use_window) {
        grid->window_size.window_start.x = 0;
        // No room for left window.
        if (source->x < parameters->left_window || (parameters->left_window == 0 && parameters->right_window == 0)) {
            grid->window_size.window_start.x = 0;
            // No room for right window.
        } else if (source->x >= grid->grid_size.nx
                                           - parameters->boundary_length - parameters->half_length - parameters->right_window) {
            grid->window_size.window_start.x = grid->grid_size.nx
                                               - parameters->boundary_length - parameters->half_length - parameters->right_window
                                               - parameters->left_window - 1;
            source->x = grid->window_size.window_nx - parameters->boundary_length
                                   - parameters->half_length - parameters->right_window - 1;
        } else {
            grid->window_size.window_start.x = source->x - parameters->left_window;
            source->x = parameters->left_window;
        }
        grid->window_size.window_start.y = 0;
        if (grid->grid_size.ny != 1) {
            if (source->y < parameters->back_window || (parameters->front_window == 0 && parameters->back_window == 0)) {
                grid->window_size.window_start.y = 0;
            } else if (source->y >= grid->grid_size.ny
                                               - parameters->boundary_length - parameters->half_length - parameters->front_window) {
                grid->window_size.window_start.y = grid->grid_size.ny
                                                   - parameters->boundary_length - parameters->half_length - parameters->front_window
                                                   - parameters->back_window - 1;
                source->y = grid->window_size.window_ny - parameters->boundary_length
                                       - parameters->half_length - parameters->front_window - 1;
            } else {
                grid->window_size.window_start.y = source->y - parameters->back_window;
                source->y = parameters->back_window;
            }
        }
    }
    // Remove traces outside the window.
    int intern_x = grid->window_size.window_nx - 2 * parameters->half_length - 2 * parameters->boundary_length;
    int intern_y = grid->window_size.window_ny - 2 * parameters->half_length - 2 * parameters->boundary_length;
    for (int i = sio->Atraces.size() - 1; i >= 0; i--) {
        int gx = (sio->Atraces[i].TraceMetaData.receiver_location_x - (int)grid->reference_point.x) /
                 (grid->cell_dimensions.dx * scale);
        int gy = (sio->Atraces[i].TraceMetaData.receiver_location_y - (int)grid->reference_point.y) /
                 (grid->cell_dimensions.dy * scale);
        if (gx < grid->window_size.window_start.x || gx >= grid->window_size.window_start.x + intern_x) {
            sio->Atraces.erase(sio->Atraces.begin() + i);
        } else if (grid->grid_size.ny != 1) {
            if (gy < grid->window_size.window_start.y || gy >= grid->window_size.window_start.y + intern_y) {
                sio->Atraces.erase(sio->Atraces.begin() + i);
            }
        }
    }
    // Set meta data.
    traces->sample_dt = sio->DM.dt;
    int sample_nt = sio->DM.nt;

    int num_elements_per_time_step = sio->Atraces.size();
    traces->trace_size_per_timestep = sio->Atraces.size();
    traces->num_receivers_in_x = x_dim.size();
    traces->num_receivers_in_y = y_dim.size();
    traces->sample_nt = sample_nt;
    grid->nt =
            int(sample_nt * traces->sample_dt /
                grid->dt); // we dont have total time , but we have the nt from the
    // segy file , so we can modify the nt accourding to the
    // ratio between the recorded dt and the suitable dt

    *total_time = sample_nt * traces->sample_dt;
    // Setup traces data to the arrays.
    traces->traces = (float *)mem_allocate(
            sizeof(float), sample_nt * num_elements_per_time_step, "traces");
    *x_position = (uint *)mem_allocate(
            sizeof(uint), num_elements_per_time_step, "traces x-position");
    *y_position = (uint *)mem_allocate(
            sizeof(uint), num_elements_per_time_step, "traces y-position");
    for (int i = 0; i < num_elements_per_time_step; i++) {
        for (int t = 0; t < sample_nt; t++) {
                traces->traces[t * num_elements_per_time_step + i] = sio->Atraces.at(i).TraceData[t];
        }
        int gx = (sio->Atraces[i].TraceMetaData.receiver_location_x - (int)grid->reference_point.x) /
                 (grid->cell_dimensions.dx * scale);
        int gy = (sio->Atraces[i].TraceMetaData.receiver_location_y - (int)grid->reference_point.y) /
                 (grid->cell_dimensions.dy * scale);
        gx -= grid->window_size.window_start.x;
        gy -= grid->window_size.window_start.y;
        (*x_position)[i] = gx + parameters->half_length + parameters->boundary_length;
        if (grid->grid_size.ny > 1) {
            (*y_position)[i] = gy + parameters->half_length + parameters->boundary_length;
        } else {
            (*y_position)[i] = 0;
        }
    }
}
