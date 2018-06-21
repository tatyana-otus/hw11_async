#pragma once
#include "queue_wrapper.h"

const size_t MAX_BULK_SIZE  = 128;
const size_t MAX_CMD_LENGTH = 64;

const size_t CIRCLE_BUFF_SIZE = 1024;
static_assert(CIRCLE_BUFF_SIZE > 4, "");


using data_type = std::vector<std::string>;
using p_data_type = data_type*;
using queue_msg_type = std::queue<p_data_type>;

using p_print_task = std::shared_ptr<queue_wrapper<p_data_type>>;

using f_msg_type = std::tuple<p_data_type, std::time_t, size_t>;
using f_msg_type_ext   = std::tuple<f_msg_type, bool*>;
using p_file_tasks = std::shared_ptr<queue_wrapper<f_msg_type_ext>>;