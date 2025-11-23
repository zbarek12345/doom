//
// Created by Wiktor on 23.11.2025.
//

#ifndef DOOM_ACTIONPERFORMER_H
#define DOOM_ACTIONPERFORMER_H
#include "new_models.h"


namespace NewModels {
	class ActionPerformer {
	public:
		virtual ~ActionPerformer() = default;

	protected:
		bool finished = false;
		float speed = 0;
public:
		ActionPerformer(uint8_t speed);

		virtual void BindTargets(void *target) = 0;

		bool IsFinished () const;

		virtual void Update(double deltaTime) = 0;
	};

	class DoorAction : public ActionPerformer {
		int16_t original_height;
		int16_t target_height;
		uint8_t wait_time;
		float wait_timer;
		Sector* target;

public:
		enum DoorType {
			Open,
			OpenClose,
			CloseOpen,
			Close
		};

		enum DoorDirection {
			Up,
			Wait,
			Down
		};
private:
DoorType DoorType_;
DoorDirection DoorDir_;
public:
		DoorAction(DoorType type, DoorDirection direction, uint8_t speed);

		void BindTargets(void *target) override;
		void Update(double deltaTime) override;

		static int16_t findAdjacentCeiling(Sector* sector);
	};
}

#endif //DOOM_ACTIONPERFORMER_H