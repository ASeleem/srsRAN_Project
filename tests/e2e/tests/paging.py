#
# Copyright 2021-2025 Software Radio Systems Limited
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the distribution.
#

"""
Paging Tests
"""
import logging
from time import sleep

from google.protobuf.empty_pb2 import Empty
from pytest import mark
from retina.client.manager import RetinaTestManager
from retina.launcher.artifacts import RetinaTestData
from retina.launcher.utils import param
from retina.protocol.fivegc_pb2_grpc import FiveGCStub
from retina.protocol.gnb_pb2_grpc import GNBStub
from retina.protocol.ue_pb2_grpc import UEStub

from .steps.configuration import configure_test_parameters, get_minimum_sample_rate_for_bandwidth
from .steps.stub import ping, ping_from_5gc, start_network, stop, ue_start_and_attach


@mark.parametrize(
    "band, common_scs, bandwidth",
    (
        param(3, 15, 10, id="band:%s-scs:%s-bandwidth:%s"),
        param(78, 30, 20, id="band:%s-scs:%s-bandwidth:%s"),
    ),
)
@mark.android
@mark.flaky(
    reruns=2,
    only_rerun=["failed to start", "Exception calling application", "Attach timeout reached", "Some packages got lost"],
)
# pylint: disable=too-many-arguments,too-many-positional-arguments
def test_cots_paging(
    retina_manager: RetinaTestManager,
    retina_data: RetinaTestData,
    ue: UEStub,
    fivegc: FiveGCStub,
    gnb: GNBStub,
    band: int,
    common_scs: int,
    bandwidth: int,
):
    """
    COTS Paging test
    """

    configure_test_parameters(
        retina_manager=retina_manager,
        retina_data=retina_data,
        band=band,
        common_scs=common_scs,
        bandwidth=bandwidth,
        sample_rate=get_minimum_sample_rate_for_bandwidth(bandwidth),
        global_timing_advance=-1,
        time_alignment_calibration="auto",
        cu_cp_inactivity_timer=1,
    )

    logging.info("Paging Test")
    start_network(ue_array=[ue], gnb=gnb, fivegc=fivegc)
    ue_attach_info_dict = ue_start_and_attach([ue], gnb.GetDefinition(Empty()), fivegc)
    ping(ue_attach_info_dict, fivegc, 10)
    sleep(5)
    ping_from_5gc(ue_attach_info_dict, fivegc, 10)
    stop(
        ue_array=[ue],
        gnb=gnb,
        fivegc=fivegc,
        retina_data=retina_data,
        warning_as_errors=False,
    )
