% this is the main function for the matlab script. It initilizes the values and does some required calculations.
% I only included code that I wrote, so some calculations are not present.

function result = mainfunc(startPoint, Box, UAV_num, speed, height, sensor_width)
   % important helper functions
   function res = genPathInShape(shape, indShapeCenter) % creates UAV path within shape
       cs = uavCoverageSpace(Polygons = {shape}, ...
           UseLocalCoordinates = false, ...
           ReferenceHeight = height, ...
           ReferenceLocation = indShapeCenter, ...
           UnitWidth = sensor_width);
       show(cs, Parent = g);
       setCoveragePattern(cs, 1, SweepAngle = 0);
       cp = uavCoveragePlanner(cs, Solver = "Exhaustive");
       res = plan(cp, startPoint);
   end   

   function res = getCenterPoint(shape) % get center coordinates of a polygon
       centroid = mean(shape, 1);
       angles = atan2(shape(:,2) - centroid(2), shape(:,1) - centroid(1));
       [~, sort_idx] = sort(angles);
       sorted_points = shape(sort_idx, :);
       x = sorted_points(:,1);
       y = sorted_points(:,2);
       x = [x; x(1)];
       y = [y; y(1)];
       total_area = polyarea(x, y);
       Cx = 0;
       Cy = 0;
       for i = 1:length(x)-1
           common = (x(i) * y(i+1) - x(i+1) * y(i));
           Cx = Cx + (x(i) + x(i+1)) * common;
           Cy = Cy + (y(i) + y(i+1)) * common;
       end
       res = [Cx / (6 * total_area), Cy / (6 * total_area), 0];
   end
   function area = getAreaUAV(UAVpath)
       maxX = max(UAVpath(2:end-1, 1, 1));
       minX = min(UAVpath(2:end-1, 1, 1));
       maxY = max(UAVpath(1, 2:end-1, 1));
       minY = min(UAVpath(1, 2:end-1, 1));
       area = sqrt((maxX-minX).^2 + (maxY-minY).^2);
   end
   function dist = getLargestSide(UAVpath)
       datap = UAVpath(2:end-1, 2, 1);
       maxY = max(datap);
       minY = min(datap);
       dist = maxY - minY;
   end
   function distance_meters = latDifferenceToMeters(len)
       radius_earth = 6371000; % meters
       delta_lat = abs(len);
       distance_meters = delta_lat * (pi / 180) * radius_earth;
   end
   function area_meters = geoAreaToMeters(are)
       radius_earth = 6371000; % meters
       res = deg2rad(are);
       area_meters = radius_earth * res;
   end
   function Total_Final_time = Calculations(max_velocity, max_acceleration, max_decceleration, width_of_view, Area, largest_side)
       R_curvature = width_of_view / 2;
       turn_velocity = sqrt(R_curvature * 4 * 9.81 * tan(25 * pi / 180));
       length_of_turn = pi * R_curvature;
       time_turn = length_of_turn / turn_velocity;
       time_slowdown = abs((turn_velocity - max_velocity) / max_decceleration);
       time_speedup = time_slowdown;
       Total_time_perturn = time_turn + time_slowdown + time_speedup;
      
       N_turns = ceil(largest_side / width_of_view) - 1;
       Total_time_correction = N_turns * Total_time_perturn;
      
       UAV_travel_dist = Area / width_of_view - N_turns * R_curvature + N_turns * (pi * R_curvature);
       Total_simple_time = UAV_travel_dist / (max_velocity * 0.9);
       Total_Final_time = Total_simple_time + Total_time_correction;
      
       fprintf('The number of turns for this shape is: %d\n', N_turns);
       fprintf('The time it takes for all the turns in shape: %.2f seconds\n', Total_time_correction);
       fprintf('The total linear path time: %.2f seconds\n', Total_simple_time);
       fprintf('The total time it took for this UAV to get through this shape with %d turns is %.2f seconds\n', N_turns, Total_Final_time);
   end
  
   % Initialization of map, variables, etc.
   limOffset = .05;
   polygon = reshape([Box(1), Box(3), Box(5), Box(7); Box(2), Box(4), Box(6), Box(8)], [], 2);
   entireShapeCenter = getCenterPoint(polygon);
  
   latLimits = [entireShapeCenter(1) - limOffset, entireShapeCenter(1) + limOffset];
   longLimits = [entireShapeCenter(2) - limOffset, entireShapeCenter(2) + limOffset];
   fig = figure;
   g = geoaxes(fig, Basemap="satellite");
   geolimits(latLimits, longLimits);
   OptimalPath = genPathInShape(polygon, entireShapeCenter);
   data = repmat({OptimalPath}, UAV_num, 1); % Replicate initial path for each UAV
   % Divide paths among UAVs
   area = getAreaUAV(OptimalPath);
   maxsearchArea = area / UAV_num;
  
   for k = 1:UAV_num
       dataPacket = data{k};
       area = getAreaUAV(dataPacket);
      
       while area > maxsearchArea
           if k > 1 && k ~= UAV_num
               prev_Packet = data{k - 1};
               if ~isequal(prev_Packet(end-1, :), dataPacket(2, :))
                   dataPacket(2, :) = [];
               else
                   dataPacket(end-1, :) = [];
               end
           elseif k == 1
               if size(dataPacket, 1) >= 2
                   dataPacket(end-1, :) = [];
               end
           else
               prev_Packet = data{k - 1};
               if ~isequal(prev_Packet(end-1, :), dataPacket(2, :))
                   dataPacket(2, :) = [];
               else
                   break;
               end
           end
           area = getAreaUAV(dataPacket);
       end
       data{k} = dataPacket;
   end
   colors = ["r" "b" "m" "w" "g" "y"];
   for j = 1:UAV_num
       hold on;
       lineinfo = data{j};
       geoplot(lineinfo(:, 1), lineinfo(:, 2), colors(j), 'LineWidth', 1.5);
       hold off;
   end
   hold on;
   geoplot(startPoint(1), startPoint(2), 'go', 'MarkerSize', 10); % Start point in green
   legend("Path", "Start Point");
   hold off;
   % Total time calculations for each UAV
   totaltimetraveled = 0;
   for p = 1:UAV_num
       UAVpath0 = data{p};
       area2 = geoAreaToMeters(getAreaUAV(UAVpath0));
       vm = latDifferenceToMeters(getLargestSide(UAVpath0));
       calc = Calculations(speed, 3, -3, sensor_width, area2, vm);
       totaltimetraveled = totaltimetraveled + calc;
   end
   fprintf('Total time traveled for all UAVs: %.2f seconds\n', totaltimetraveled/3);
   pause(.1);
end
