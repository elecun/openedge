
from django.conf import settings

'''
User global variable definitions for Divelink
'''
def context_processors(request):
    return {
        'system':{ 
            'title':'DIVELINK',
            'company':"Insitute for Advanced Engineering",
            'version':"0.1.0"
            },
        'frontend':{
            'status_load_min':0,
            'status_load_max':1000,
            'status_rpm_min':0,
            'status_rpm_max':1000,
            'status_temperature1_min':0,
            'status_temperature1_max':100,
            'status_temperature2_min':0,
            'status_temperature2_max':100,
            'status_temperature3_min':0,
            'status_temperature3_max':100,
        },
        'backend':{

        },
        'menu_ko':{
            'menu_title_map':"Map",
            'menu_title_rsu':"RSU",
            'menu_title_monitor':"Monitor",
            'menu_title_message':"Message",
            'menu_title_setting':"Setting",
            'menu_title_datalog':"Data Log",
            'menu_title_info':"Info",

            'rsu_add_new':"Add New RSU",
            'log_add_new':"Add New Log",

            'message_delete_rsu_selected':"선택한 장치를 삭제하시겠습니까?",
            'message_activate_rsu_selected':"선택한 장치를 활성화하시겠습니까?",
            'message_deactivate_rsu_selected':"선택한 장치를 비활성화하시겠습니까?",

            'message_delete_datalog_selected':"선택한 로그를 삭제하시겠습니까?",
            'message_close_datalog_selected':"현재 로그 종료 시각을 저장하겠습니까?",

            'message_setting_apply':"시스템 설정을 적용하시겠습니까?",


            'settings_load':"불러오기",
            'settings_save' : "설정저장",
            'settings_apply' : "PLC 적용",
            'settings_new' : "New",
            'settings_reserve' : "시험예약",
            'start_step' : "시험시작",
            'pause_step' : "시험일시정지",
            'stop_step' : "시험중지",
            'zeroset' : "영점설정",
            'settings_name':"설정명",
            'settings_general_ratio' : "Ratio조정",
            'settings_general_machinename':"장비명",
            'settings_general_jsno':"JS No.",
            'settings_general_productsize':"제품 사이즈(ø)",
            'settings_general_rollersize':"구동롤러 사이즈(ø)",
            'settings_general_jsno':"JS No.",
            'settings_general_wtime':"시험시간(hr)",
            'settings_general_update_interval':"측정 주기(sec)",
            'settings_general_rpm':"제한속도(rpm)",
            'settings_general_rpm_max':"max",
            'settings_general_rpm_max_count':"count",
            'settings_general_temp':"제한온도(℃)",
            'settings_general_temp_min':"min",
            'settings_general_temp_max':"max",
            'settings_general_temp_min_count':"count",
            'settings_general_temp_max_count':"count",
            'settings_general_vload':"수직하중(kgf)",
            'settings_general_load':"제한하중(kgf)",
            'settings_general_load_min':"min",
            'settings_general_load_max':"max",
            'settings_general_load_min_count':"count",
            'settings_general_load_max_count':"count",
            'status_current':"현재 상태",
            'status_manual_control':"수동 제어",
            'status_manual_control_main_speed':"Main 속도",
            'status_manual_control_main_speed_value':"Main 속도설정",
            'status_manual_control_cw':"CW",
            'status_manual_control_ccw':"CCW",
            'status_manual_control_stop':"정지",
            'status_manual_control_motor':"모터 ON/OFF",
            'status_manual_control_sideload':"측면하중 ON/OFF",
            'status_manual_control_cylinder_move':"실린더 이동",
            'status_manual_control_cylinder_up':"실린더 UP",
            'status_manual_control_cylinder_down':"실린더 DOWN",
            'status_manual_control_cylinder_stop':"실린더 정지",
            'status_load':"현재 하중(kgf)",
            'status_rpm':"현재 속도(rpm)",
            'status_temperature_1':"P1 온도(℃)",
            'status_temperature_2':"P2 온도(℃)",
            'status_temperature_3':"P3 온도(℃)",
            'status_inprogress_step':"현재 STEP",
            'status_inprogress_time':"현재 진행시간",
            'chart_open':"데이터 열기",
            'chart_raw_download':"CSV 내보내기",
            'chart_image_download':"이미지 내보내기",
            'settings_reserve_datetime':"실행 날짜/시각",
            'settings_reserve_add':"예약 적용",
            'settings_reserve_list':"예약 리스트",
            'message_update_success':"정상적으로 설정을 저장 및 업데이트 하였습니다.",
            'message_update_error_no_target':"업데이트할 설정 대상이 지정되지 않았습니다.",
            'message_start_error_no_target':"로드된 설정이 없습니다. 시험을 시작할 수 없습니다.",
            'message_reserve_error_no_target':"로드된 설정이 없습니다. 시험을 예약할 수 없습니다.",
            'message_load_selected':"선택한 설정을 불러오기 하시겠습니까?",
            'message_delete_setting_selected':"선택한 설정을 삭제 하시겠습니까?",
            'message_delete_reserve_selected':"선택한 예약을 삭제 하시겠습니까?",
            'message_start_steps':"시험을 시작하시겠습니까?",
            'message_stopped_steps':"시험이 종료되었습니다.",
            'message_paused_steps':"시험이 일시 정지 되었습니다.",
            'message_emergency':"응급 상황이 감지되었습니다.",
            'chart_title':"",
        }
    }


