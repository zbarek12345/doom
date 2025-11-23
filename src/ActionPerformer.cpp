//
// Created by Wiktor on 23.11.2025.
//

#include "headers/ActionPerformer.h"

#include "headers/Sector.h"


NewModels::ActionPerformer::ActionPerformer(uint8_t speed) {
	this->speed = speed;
	this->finished = false;
}

bool NewModels::ActionPerformer::IsFinished() const {
	return finished;
}

NewModels::DoorAction::DoorAction(DoorType type, DoorDirection direction, uint8_t speed) : ActionPerformer(speed),
	original_height(0),
	target_height(0),
	target(nullptr) {
	this->DoorType_ = type;
	this->DoorDir_ = direction;

	switch (type) {
		case OpenClose: // rDoor: 4, 29, 90, 63, 108*, 111*, 105*, 114*
			this->wait_time = 4;
			break;
		case Open: // rDoor: 2, 103, 86, 61, 109*, 112*, 106*, 115* & mDoor: 31, 32-34, 118*
			this->wait_time = 0; // No wait time for Open-only
			break;
		case Close: // rDoor: 3, 50, 75, 42, 110*, 113*, 107*, 116*
			this->wait_time = 0; // No wait time for Close-only
			break;
		case CloseOpen: // rDoor: 16, 76
			this->wait_time = 30; // Close, then opens after 30 tics (1 sec = ~35 tics)
			break;
		// Manual doors (1, 117*) are Open/Close and will be handled by the activation system
		// which determines whether to start opening or closing.
		default:
			this->wait_time = 0;
			break;
	}

	wait_timer = 0.;
}


void NewModels::DoorAction::BindTargets(void *target) {
	this->target = static_cast<Sector *>(target);
	this->original_height = this->target->floor_height;
	this->target_height = findAdjacentCeiling(this->target);
}

void NewModels::DoorAction::Update(double deltaTime) {
	assert(target != nullptr);

	if (this->DoorDir_ == Up) {

		this->target->ceil_height += std::max(1., this->speed * deltaTime);
		if (this->target->ceil_height >= this->target_height) {
			this->target->ceil_height = this->target_height;
			this->DoorDir_ = Wait;
			this->wait_timer = 0.0f;

			// If Open-only (type 2/31 etc.), it's finished after reaching target
			if (this->DoorType_ == Open || this->DoorType_ == CloseOpen) {
				this->finished = true;
			}
		}
	} else if (this->DoorDir_ == Down) {
		this->target->ceil_height -= std::max(1., this->speed * deltaTime);
		if (this->target->ceil_height <= this->original_height) {
			this->target->ceil_height = this->original_height;
			// Finished when fully closed
			this->DoorDir_ = Wait;
			this->wait_timer = 0.0f;
			// If Close-Open (type 16/76), it needs to rest, then open again
			if (this->DoorType_ == Close || this->DoorType_ == OpenClose) {
				this->finished = true;
			}
		}
	}

	if (this->DoorDir_ == Wait) {
		this->wait_timer += deltaTime;

		if (this->wait_timer >= wait_time) {
			// Door has rested, now perform the next action
			if (this->DoorType_ == OpenClose) {
				this->DoorDir_ = Down;
				this->target_height = original_height;
			}else {
				this->DoorDir_ = Up;
				this->target_height = findAdjacentCeiling(target);
			}
		}
	}

}

int16_t NewModels::DoorAction::findAdjacentCeiling(Sector *sector) {
	int16_t lac = SHRT_MAX;
	for (auto& neighbour : sector->neighbors) {
		lac = std::min(neighbour->ceil_height, lac);
	}
	return lac;
}

NewModels::LiftAction::LiftAction(uint8_t speed) : ActionPerformer(speed), target(nullptr), original_height(0),
                                                   target_height(0),
                                                   wait_time(0),
                                                   wait_timer(0),
                                                   LiftDir_(Down){}

void NewModels::LiftAction::BindTargets(void *target) {
	this->target = static_cast<Sector *>(target);
	this->original_height = this->target->floor_height;
	this->target_height = findAdjacentFloor(this->target);
}

void NewModels::LiftAction::Update(double deltaTime) {
	assert(target != nullptr);
	if (LiftDir_ == Down) {
		this->target->floor_height -= std::max(1., this->speed * deltaTime);
		if (this->target->floor_height <= this->target_height) {
			this->target->floor_height = this->target_height;
			this->LiftDir_ = Wait;
			this->wait_timer = 0.0f;
		}
	}

	else if (LiftDir_ == Up) {
		this->target->floor_height += std::max(1., this->speed * deltaTime);
		if (this->target->floor_height >= this->original_height) {
			this->target->floor_height = this->original_height;
			finished = true;
		}
	}

	else {
		this->wait_timer += deltaTime;
		if (this->wait_timer >= wait_time) {
			this->LiftDir_ = Up;
		}
	}
}

int16_t NewModels::LiftAction::findAdjacentFloor(const Sector *sector) {
	int16_t haf = SHRT_MAX;
	for (auto& neighbour : sector->neighbors) {
		haf = std::min(haf, neighbour->floor_height);
	}
	return haf;
}
