/*
 * Copyright (C) 2016  Nexell Co., Ltd.
 * Author: Sungwoo, Park <swpark@nexell.co.kr>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>

#include <nxs-res-manager.h>
#include <nxs-v4l2.h>
#include <nx-drm.h>
#include <nxs-v4l2-test-common.h>

int main(int argc, char *argv[])
{
	int ret;
	int fd;
	struct nxs_request_function req;
	struct nxs_query_function query;
	int v4l2_fd;
	char dev_name[128] = {0, };
	struct nxs_v4l2_test_common_option option;

	ret = nxs_v4l2_test_common_get_option(argc, argv,
					      NXS_V4L2_TEST_TYPE_CAPTURE,
					      &option);
	if (ret)
		return ret;

	/* TODO: add vip module number */
	nxs_v4l2_test_common_print_option(&option);

	fd = nxs_res_manager_open();
	if (fd < 0) {
		fprintf(stderr, "failed to nxs_res_manager_open\n");
		return -ENODEV;
	}

	bzero(&req, sizeof(req));
	strcpy(req.name, "vipclipper-dmaw");
	req.count = 2;
	req.array[0] = NXS_FUNCTION_VIP_CLIPPER;
	req.array[1] = option.module;
	req.array[2] = NXS_FUNCTION_DMAW;
	req.array[3] = NXS_FUNCTION_ANY;

	ret = nxs_res_manager_request(fd, &req);
	if (ret) {
		fprintf(stderr, "failed to nxs_res_manager_request\n");
		return ret;
	}

	bzero(&query, sizeof(query));
	query.handle = req.handle;
	query.query = NXS_FUNCTION_QUERY_DEVINFO;

	ret = nxs_res_manager_query(fd, &query);
	if (ret) {
		fprintf(stderr, "failed to nxs_res_manager_query\n");
		return ret;
	}

	sprintf(dev_name, "/dev/%s", query.devinfo.dev_name);
	v4l2_fd = open(dev_name, O_RDWR);
	if (v4l2_fd < 0) {
		fprintf(stderr, "failed to open %s\n", dev_name);
		return -ENODEV;
	}

	nxs_v4l2_test_common_capture(v4l2_fd, &option);

	close(v4l2_fd);

	nxs_res_manager_remove(fd, req.handle);
	nxs_res_manager_close(fd);

	return 0;
}
