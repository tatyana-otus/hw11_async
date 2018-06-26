#pragma once
#include "queue_wrapper.h"

const size_t MAX_BULK_SIZE  = 128;
const size_t MAX_CMD_LENGTH = 64;

const size_t MAX_QUEUE_SIZE = 1024;

using data_type = std::vector<std::string>;

using p_data_type = std::shared_ptr<data_type>;
using p_tasks_t = queue_wrapper<std::shared_ptr<data_type>>;

using f_msg_type = std::tuple<p_data_type, std::time_t, size_t>;
using f_tasks_t = queue_wrapper<f_msg_type>;