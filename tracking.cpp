#include "tracking.h"

// string VIDEO_FILE_PATH = "/Users/matthieu/Movies/tracking/Basket.mp4";
float AREA_OVERLAP_THRESHOLD = 0.5;

bool overlap(Rect new_rectangle, vector<Rect> tracking_rectangles)
{

	int tracked_players_count = tracking_rectangles.size();
	for (int player_index = 0; player_index < tracked_players_count; player_index++)
	{
		Rect player_rectangle = tracking_rectangles[player_index];
		int min_area = min(new_rectangle.area(), player_rectangle.area());
		int intersection_area = (player_rectangle & new_rectangle).area();
		if (intersection_area > min_area * AREA_OVERLAP_THRESHOLD)
			return true;
	}
	return false;
}

void add_trackers(vector<Rect> &detected_rectangles, vector<Rect> &matched_rectangles, vector<Ptr<TrackerCSRT>> &player_trackers, Mat &frame)
{
	for (auto iterator = detected_rectangles.begin(); iterator != detected_rectangles.end(); iterator++)
	{
		Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
		tracker->init(frame, *(iterator));
		if (player_trackers.size() == 0)
		{
			cout << "no matched rectangles" << endl;
			player_trackers.push_back(tracker);
			matched_rectangles.push_back(*(iterator));
			cout << "Matched rectangles count : " << matched_rectangles.size() << endl;
			cout << "detected rectangles count : " << detected_rectangles.size() << endl;
			return;
		}
		if (!overlap(*(iterator), matched_rectangles))
		{
			Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
			tracker->init(frame, *(iterator));
			player_trackers.push_back(tracker);
			matched_rectangles.push_back(*(iterator));
		}
	}
}

// void add_trackers(vector<Rect> &hog_rectangles, vector<Rect> &tracking_rectangles, vector<Ptr<TrackerCSRT>> player_trackers){
// 	int hog_rectangles_count = hog_rectangles.size();
// 	for (int rectangle_index = 0; rectangle_index < hog_rectangles_count; rectangle_index++)
// 		{
// 			Rect new_rectangle = hog_rectangles[rectangle_index];
// 			if (overlap(new_rectangle, tracking_rectangles) == true) continue;

// 			tracking_rectangles.push_back(new_rectangle);
// 			Ptr<TrackerCSRT> tracker = TrackerCSRT::create();
// 			tracker->init(frame, *(iterator));
// 			player_trackers

// 		}
// 	return;
// }

void record_tracking_rectangles(string VIDEO_FILE_PATH, vector<vector<Rect>> &hog_frame_rectangles, vector<vector<Rect>> &matched_rectangles)
{ // List of tracker types in OpenCV 3.4.1

	VideoCapture video(VIDEO_FILE_PATH);

	// Check if camera opened successfully
	if (!video.isOpened())
		cout << "Error opening video stream or file" << endl;
	Mat frame;
	video >> frame;

	int frame_index = 0;
	auto hog_rectangles_iterator = hog_frame_rectangles.begin();
	vector<Rect> detected_rectangles = *(hog_rectangles_iterator);
	vector<Ptr<TrackerCSRT>> player_trackers;
	if (detected_rectangles.size() == 0)
	{
		cout << "No rectangles for initital tracking, please select one." << endl;
		Rect window = selectROI("select tracker", frame);
		detected_rectangles.push_back(window);
		destroyWindow("select tracker");
	}
	vector<Rect> frame_matched_rectangles;

	cout << "Reading frame " << frame_index << endl;
	add_trackers(detected_rectangles, frame_matched_rectangles, player_trackers, frame);
	matched_rectangles.push_back(frame_matched_rectangles);

	while (1)
	{

		frame_index += 1;
		video.read(frame);

		// If the frame is empty, break immediately
		if (frame.empty())
		{
			cout << "problem" << endl;
			break;
		}

		cout << "Matched rectangles count : " << frame_matched_rectangles.size() << endl;
		cout << "detected rectangles count : " << detected_rectangles.size() << endl;
		assert(player_trackers.size() == frame_matched_rectangles.size());
		// Update trackers
		for (int rectangle_index = 0; rectangle_index < frame_matched_rectangles.size(); rectangle_index++)
		{
			Ptr<TrackerCSRT> tracker = player_trackers[rectangle_index];
			Rect2d window = (Rect2d)frame_matched_rectangles[rectangle_index];
			tracker->update(frame, window);
			frame_matched_rectangles[rectangle_index] = (Rect)window;
			rectangle(frame, frame_matched_rectangles[rectangle_index], Scalar(255, 0, 0), 2, 1);
		}

		vector<Rect> hog_rectangles = *(hog_rectangles_iterator);
		add_trackers(hog_rectangles, frame_matched_rectangles, player_trackers, frame);
		matched_rectangles.push_back(frame_matched_rectangles);

		int hog_rectangles_count = hog_rectangles.size();
		for (int rectangle_index = 0; rectangle_index < hog_rectangles_count; rectangle_index++)
		{
			Rect2d window = hog_rectangles[rectangle_index];
			rectangle(frame, window, Scalar(0, 255, 0), 2, 1);
		}

		imshow("Tracking", frame);

		frame_index++;
		hog_rectangles_iterator++;

		// Press ESC to stop
		if (waitKey(1) == 27)
			break;

	}
	video.release();
	destroyWindow("Tracking");
}

void record_hog_rectangles(string video_file_path, vector<vector<Rect>> &hog_frame_rectangles)
{
	float weight_threshold = 0.0;
	int frame_index = 0;
	VideoCapture video(video_file_path);

	// Check if camera opened successfully
	if (!video.isOpened())
	{
		cout << "Error opening video stream or file" << endl;
	};
	Mat frame;
	video >> frame;

	// Pedestrian recordor
	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	while (1)
	{

		// Capture frame-by-frame
		video.read(frame);
		// cout << "frame index : " << frame_index << endl;

		// If the frame is empty, break immediately
		if (frame.empty())
		{
			cout << "Coudl not read frame " << frame_index << endl;
			break;
		}

		vector<Rect> recordion_rectangles;
		vector<double> weights;

		Mat gray_frame;
		cvtColor(frame, gray_frame, COLOR_BGR2GRAY);
		hog.detectMultiScale(gray_frame, recordion_rectangles, weights);
		int hog_rectangle_count = recordion_rectangles.size();
		// cout << "recordion complete, number of reactangles deteceted : " << hog_rectangle_count << endl;

		hog_frame_rectangles.push_back(recordion_rectangles);
		/// draw recordions
		for (size_t i = 0; i < hog_rectangle_count; i++)
		{
			if (weights[i] < weight_threshold)
				continue;
			rectangle(gray_frame, recordion_rectangles[i], cv::Scalar(0, 0, 255), 3);
		}

		// Display the resulting frame
		imshow("HOG detection", gray_frame);

		// Press ESC to stop
		if (waitKey(1) == 27)
			break;

		frame_index++;
	}
	video.release();
	destroyWindow("HOG detection");

}

// int main()
// {
// 	VideoCapture video(VIDEO_FILE_PATH);

// 	// Check if camera opened successfully
// 	if (!video.isOpened())
// 	{
// 		cout << "Error opening video stream or file" << endl;
// 		return -1;
// 	};

// 	int frame_count = video.get(CAP_PROP_FRAME_COUNT);
// 	cout << "Video of " << frame_count << " frames loaded" << endl;
// 	vector<vector<Rect>> hog_frame_rectangles;
// 	vector<vector<Rect>> tracking_rectangles;

// 	record_hog_rectangles(VIDEO_FILE_PATH, hog_frame_rectangles);
// 	int rectangle_count = hog_frame_rectangles[10].size();

// 	record_tracking_rectangles(VIDEO_FILE_PATH, hog_frame_rectangles, tracking_rectangles);

// 	// cout << "number of rectangles for frame " << 10 << " : " << rectangle_count << endl;
// }